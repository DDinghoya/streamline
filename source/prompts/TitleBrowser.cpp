/****************************************************************************
 * TitleBrowser
 * USB Loader GX 2009
 *
 * TitleBrowser.cpp   *giantpune*
 ***************************************************************************/

#include <dirent.h>


#include "language/gettext.h"
#include "libwiigui/gui.h"
#include "libwiigui/gui_customoptionbrowser.h"
#include "prompts/PromptWindows.h"
#include "prompts/ProgressWindow.h"
#include "network/networkops.h"
#include "network/http.h"
#include "filelist.h"
#include "listfiles.h"
#include "settings/cfg.h"
#include "sys.h"
#include "menu.h"
#include "audio.h"
#include "wad/wad.h"
#include "xml/xml.h"
#include "wad/nandtitle.h"
#include "../usbloader/utils.h"
#include "../gecko.h"

u32 titleCnt;
extern u32 infilesize;
extern u32 uncfilesize;
extern char wiiloadVersion[2];
#include <zlib.h>
#include "settings/cfg.h"
#include "unzip/unzip.h"
#include "unzip/miniunz.h"


/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

/*** Extern variables ***/
extern GuiWindow * mainWindow;
extern u8 shutdown;
extern u8 reset;
extern u32 infilesize;
extern wchar_t *gameFilter;


/********************************************************************************
* TitleBrowser- opens a browser with a list of installed Titles
* relies on code from any title deleter.
*********************************************************************************/
int TitleBrowser()
{

    u32 num_titles;
    u32 num_sys_titles;
    s32 ret = -1;
    u64 *titleList = NULL;

    ISFS_Initialize();//initialize for "titles.Exists()"

    // Get count of titles of the good titles
    num_titles = titles.SetType( 0x10001 );
    u32 n = num_titles;
    for ( u32 i = 0; i < n; i++ )
    {
        u64 tid = titles.Next();
        if ( !tid )
        {
            break;
        }

        //remove ones not actually installed on the nand
        if ( !titles.Exists( tid ) )
        {
            num_titles--;
        }
    }

    // Get count of system titles
    num_sys_titles = titles.SetType( 0x10002 );
    n = num_sys_titles;
    for ( u32 i = 0; i < n; i++ )
    {
        u64 tid = titles.Next();
        if ( !tid )
        {
            break;
        }
        //these can't be booted anyways
        if ( TITLE_LOWER( tid ) == 0x48414741 || TITLE_LOWER( tid ) == 0x48414141 || TITLE_LOWER( tid ) == 0x48414641 )
        {
            num_sys_titles--;
            continue;
        }

        //these aren't installed on the nand
        if ( !titles.Exists( tid ) )
        {
            num_sys_titles--;
        }
    }

    //make a list of just the tids we are adding to the titlebrowser
    titleList = ( u64* )memalign( 32, ( num_titles + num_sys_titles ) * sizeof( u64 ) );
    if ( !titleList )
    {
        gprintf( "TitleBrowser(): out of memory!\n" );
        return -1;
    }
    customOptionList options3( num_titles + num_sys_titles + 1 );
    //write the titles on the option browser

    u32 i = 0;
    titles.SetType( 0x10001 );
    //first add the good stuff
    while ( i < num_titles )
    {
        u64 tid = titles.Next();
        if ( !tid )
        {
            gprintf( "shit happened3\n" );
            break;
        }
        //gprintf("[ %u] tid: %016llx\t%s\n", i, tid, titles.NameOf( tid ) );

        if ( !titles.Exists( tid ) )
        {
            continue;
        }

        char id[ 5 ];
        titles.AsciiTID( tid, ( char* )&id );

        char* name = titles.NameOf( tid );

        options3.SetName( i, "%s", id );
        options3.SetValue( i, "%s", name ? titles.NameOf( tid ) : "Unknown" );
        titleList[ i ] = tid;
        i++;
    }

    titles.SetType( 0x10002 );
    while ( i < num_sys_titles + num_titles )
    {
        u64 tid = titles.Next();
        if ( !tid )
        {
            break;
        }
        if ( TITLE_LOWER( tid ) == 0x48414741 || TITLE_LOWER( tid ) == 0x48414141 || TITLE_LOWER( tid ) == 0x48414641 )
            continue;

        if ( !titles.Exists( tid ) )
        {
            continue;
        }

        char id[ 5 ];
        titles.AsciiTID( tid, ( char* )&id );
        char* name = titles.NameOf( tid );

        options3.SetName( i, "%s", id );
        options3.SetValue( i, "%s", name ? titles.NameOf( tid ) : "Unknown" );
        titleList[ i ] = tid;
        i++;
    }

    ISFS_Deinitialize();


    if ( i == num_titles + num_sys_titles )
    {
        options3.SetName( i, " " );
        options3.SetValue( i, "%s", tr( "Wii Settings" ) );
    }

    bool exit = false;
    int total = num_titles + num_sys_titles;

    if ( IsNetworkInit() )
        ResumeNetworkWait();

    GuiSound btnSoundOver( button_over_pcm, button_over_pcm_size, Settings.sfxvolume );
    // because destroy GuiSound must wait while sound playing is finished, we use a global sound
    if ( !btnClick2 ) btnClick2 = new GuiSound( button_click2_pcm, button_click2_pcm_size, Settings.sfxvolume );
    //  GuiSound btnClick(button_click2_pcm, button_click2_pcm_size, Settings.sfxvolume);

    char imgPath[100];

    snprintf( imgPath, sizeof( imgPath ), "%sbutton_dialogue_box.png", CFG.theme_path );
    GuiImageData btnOutline( imgPath, button_dialogue_box_png );
    snprintf( imgPath, sizeof( imgPath ), "%sgamesettings_background.png", CFG.theme_path );
    GuiImageData settingsbg( imgPath, settings_background_png );

    GuiTrigger trigA;
    trigA.SetSimpleTrigger( -1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A );
    GuiTrigger trigHome;
    trigHome.SetButtonOnlyTrigger( -1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, 0 );
    GuiTrigger trigB;
    trigB.SetButtonOnlyTrigger( -1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B );

    GuiText titleTxt( tr( "Title Launcher" ), 28, ( GXColor ) {0, 0, 0, 255} );
    titleTxt.SetAlignment( ALIGN_CENTRE, ALIGN_TOP );
    titleTxt.SetPosition( 12, 40 );
    titleTxt.SetMaxWidth( 356, SCROLL_HORIZONTAL );

    GuiImage settingsbackground( &settingsbg );
    GuiButton settingsbackgroundbtn( settingsbackground.GetWidth(), settingsbackground.GetHeight() );
    settingsbackgroundbtn.SetAlignment( ALIGN_LEFT, ALIGN_TOP );
    settingsbackgroundbtn.SetPosition( 0, 0 );
    settingsbackgroundbtn.SetImage( &settingsbackground );

    GuiText cancelBtnTxt( tr( "Back" ), 22, THEME.prompttext );
    cancelBtnTxt.SetMaxWidth( btnOutline.GetWidth() - 30 );
    GuiImage cancelBtnImg( &btnOutline );
    if ( Settings.wsprompt == yes )
    {
        cancelBtnTxt.SetWidescreen( CFG.widescreen );
        cancelBtnImg.SetWidescreen( CFG.widescreen );
    }
    GuiButton cancelBtn( &cancelBtnImg, &cancelBtnImg, 2, 3, 180, 400, &trigA, &btnSoundOver, btnClick2, 1 );
    cancelBtn.SetScale( 0.9 );
    cancelBtn.SetLabel( &cancelBtnTxt );
    cancelBtn.SetTrigger( &trigB );

    u8 scrollbaron = 0;
    if ( total + 1 > 9 )
        scrollbaron = 1;

    GuiCustomOptionBrowser optionBrowser3( 396, 280, &options3, CFG.theme_path, "bg_options_gamesettings.png", bg_options_settings_png, scrollbaron, 200 );
    optionBrowser3.SetPosition( 0, 90 );
    optionBrowser3.SetAlignment( ALIGN_CENTRE, ALIGN_TOP );

    snprintf( imgPath, sizeof( imgPath ), "%sWifi_btn.png", CFG.theme_path );
    GuiImageData wifiImgData( imgPath, Wifi_btn_png );
    GuiImage wifiImg( &wifiImgData );
    if ( Settings.wsprompt == yes )
    {
        wifiImg.SetWidescreen( CFG.widescreen );
    }
    GuiButton wifiBtn( wifiImg.GetWidth(), wifiImg.GetHeight() );
    wifiBtn.SetImage( &wifiImg );
    wifiBtn.SetPosition( 100, 400 );
    wifiBtn.SetEffectGrow();
    wifiBtn.SetAlpha( 80 );
    wifiBtn.SetTrigger( &trigA );

    GuiTrigger trigZ;
    trigZ.SetButtonOnlyTrigger( -1, WPAD_NUNCHUK_BUTTON_Z | WPAD_CLASSIC_BUTTON_ZL, PAD_TRIGGER_Z );

    GuiButton screenShotBtn( 0, 0 );
    screenShotBtn.SetPosition( 0, 0 );
    screenShotBtn.SetTrigger( &trigZ );

    HaltGui();
    GuiWindow w( screenwidth, screenheight );
    w.Append( &screenShotBtn );
    w.Append( &settingsbackgroundbtn );
    w.Append( &titleTxt );
    w.Append( &cancelBtn );
    w.Append( &wifiBtn );
    w.Append( &optionBrowser3 );
    mainWindow->Append( &w );



    ResumeGui();

    while ( !exit )
    {
        VIDEO_WaitVSync();

        if ( shutdown == 1 ) Sys_Shutdown();
        if ( reset == 1 ) Sys_Reboot();

        else if ( wifiBtn.GetState() == STATE_CLICKED )
        {
            ResumeNetworkWait();
            wifiBtn.ResetState();
        }

        if ( IsNetworkInit() )
        {
            wifiBtn.SetAlpha( 255 );
        }

        ret = optionBrowser3.GetClickedOption();

        if ( ret > -1 )
        {   //if a click happened

            if ( ret < total )
            {
                //set the title's name, number, ID to text
                char text[ 0x100 ];
                char id[ 5 ];
                titles.AsciiTID( titleList[ ret ], ( char* )&id );

                snprintf( text, sizeof( text ), "%s : %s", id, titles.NameOf( titleList[ ret ] ) );

                //prompt to boot selected title
                if ( WindowPrompt( tr( "Boot?" ), text, tr( "OK" ), tr( "Cancel" ) ) )
                {   //if they say yes
                    CloseXMLDatabase();
                    ExitGUIThreads();
                    ShutdownAudio();
                    StopGX();
                    WII_Initialize();
                    WII_LaunchTitle( titleList[ ret ] );
                    //this really shouldn't be needed because the title will be booted
                    exit = true;
                    break;
                }
                else
                {
                    //if they said no to booting the title
                    ret = -1;
                    optionBrowser3.ResetState();
                }

            }
            else if ( ret == total )
            { //if they clicked to go to the wii settings
                CloseXMLDatabase();
                ExitGUIThreads();
                ShutdownAudio();
                StopGX();
                WII_Initialize();
                WII_ReturnToSettings();
            }
        }
#if 0
        if ( infilesize > 0 )
        {

            char filesizetxt[50];
            char temp[50];
            char filepath[100];
//              u32 read = 0;

            //make sure there is a folder for this to be saved in
            struct stat st;
            snprintf( filepath, sizeof( filepath ), "%s/wad/", bootDevice );
            if ( stat( filepath, &st ) != 0 )
            {
                if ( subfoldercreate( filepath ) != 1 )
                {
                    WindowPrompt( tr( "Error !" ), tr( "Can't create directory" ), tr( "OK" ) );
                }
            }
            snprintf( filepath, sizeof( filepath ), "%s/wad/tmp.tmp", bootDevice );


            if ( infilesize < MB_SIZE )
                snprintf( filesizetxt, sizeof( filesizetxt ), tr( "Incoming file %0.2fKB" ), infilesize / KB_SIZE );
            else
                snprintf( filesizetxt, sizeof( filesizetxt ), tr( "Incoming file %0.2fMB" ), infilesize / MB_SIZE );

            snprintf( temp, sizeof( temp ), tr( "Load file from: %s ?" ), GetIncommingIP() );

            int choice = WindowPrompt( filesizetxt, temp, tr( "OK" ), tr( "Cancel" ) );
            gprintf( "\nchoice:%d", choice );

            if ( choice == 1 )
            {

                u32 read = 0;
                u8 *temp = NULL;
                int len = NETWORKBLOCKSIZE;
                temp = ( u8 * ) malloc( infilesize );

                bool error = false;
                u8 *ptr = temp;
                gprintf( "\nrecieving shit" );
                while ( read < infilesize )
                {

                    ShowProgress( tr( "Receiving file from:" ), GetIncommingIP(), NULL, read, infilesize, true );

                    if ( infilesize - read < ( u32 ) len )
                        len = infilesize - read;
                    else
                        len = NETWORKBLOCKSIZE;

                    int result = network_read( ptr, len );

                    if ( result < 0 )
                    {
                        WindowPrompt( tr( "Error while transfering data." ), 0, tr( "OK" ) );
                        error = true;
                        break;
                    }
                    if ( !result )
                    {
                        gprintf( "\n!RESULT" );
                        break;
                    }
                    ptr += result;
                    read += result;
                }
                ProgressStop();

                char filename[101];
                char tmptxt[200];



                //bool installWad=0;
                if ( !error )
                {
                    gprintf( "\nno error yet" );

                    network_read( ( u8* ) &filename, 100 );
                    gprintf( "\nfilename: %s", filename );

                    // Do we need to unzip this thing?
                    if ( wiiloadVersion[0] > 0 || wiiloadVersion[1] > 4 )
                    {
                        gprintf( "\nusing newer wiiload version" );

                        if ( uncfilesize != 0 ) // if uncfilesize == 0, it's not compressed
                        {
                            gprintf( "\ntrying to uncompress" );
                            // It's compressed, uncompress
                            u8 *unc = ( u8 * ) malloc( uncfilesize );
                            uLongf f = uncfilesize;
                            error = uncompress( unc, &f, temp, infilesize ) != Z_OK;
                            uncfilesize = f;

                            free( temp );
                            temp = unc;
                        }
                    }

                    if ( !error )
                    {
                        sprintf( tmptxt, "%s", filename );
                        //if we got a wad
                        if ( strcasestr( tmptxt, ".wad" ) )
                        {
                            FILE *file = fopen( filepath, "wb" );
                            fwrite( temp, 1, ( uncfilesize > 0 ? uncfilesize : infilesize ), file );
                            fclose( file );

                            sprintf( tmptxt, "%s/wad/%s", bootDevice, filename );
                            if ( checkfile( tmptxt ) )remove( tmptxt );
                            rename( filepath, tmptxt );

                            //check and make sure the wad we just saved is the correct size
                            u32 lSize;
                            file = fopen( tmptxt, "rb" );

                            // obtain file size:
                            fseek ( file , 0 , SEEK_END );
                            lSize = ftell ( file );

                            rewind ( file );
                            if ( lSize == ( uncfilesize > 0 ? uncfilesize : infilesize ) )
                            {
                                gprintf( "\nsize is ok" );
                                int pick = WindowPrompt( tr( " Wad Saved as:" ), tmptxt, tr( "Install" ), tr( "Uninstall" ), tr( "Cancel" ) );
                                //install or uninstall it
                                if ( pick == 1 )
                                {
                                    HaltGui();
                                    w.Remove( &titleTxt );
                                    w.Remove( &cancelBtn );
                                    w.Remove( &wifiBtn );
                                    w.Remove( &optionBrowser3 );
                                    ResumeGui();

                                    Wad_Install( file );

                                    HaltGui();
                                    w.Append( &titleTxt );
                                    w.Append( &cancelBtn );
                                    w.Append( &wifiBtn );
                                    w.Append( &optionBrowser3 );
                                    ResumeGui();

                                }
                                if ( pick == 2 )Wad_Uninstall( file );
                            }
                            else gprintf( "\nBad size" );
                            //close that beast, we're done with it
                            fclose ( file );

                            //do we want to keep the file in the wad folder
                            if ( WindowPrompt( tr( "Delete ?" ), tmptxt, tr( "Delete" ), tr( "Keep" ) ) != 0 )
                                remove( tmptxt );
                        }
                        else
                        {
                            WindowPrompt( tr( "ERROR:" ), tr( "Not a WAD file." ), tr( "OK" ) );
                        }
                    }
                }



                if ( error || read != infilesize )
                {
                    WindowPrompt( tr( "Error:" ), tr( "No data could be read." ), tr( "OK" ) );


                }
                if ( temp )free( temp );
            }



            CloseConnection();
            ResumeNetworkWait();
        }
#endif
        if ( cancelBtn.GetState() == STATE_CLICKED )
        {
            //break the loop and end the function
            exit = true;
            ret = -10;
        }
        else if ( screenShotBtn.GetState() == STATE_CLICKED )
        {
            gprintf( "\n\tscreenShotBtn clicked" );
            screenShotBtn.ResetState();
            ScreenShot();
            gprintf( "...It's easy, mmmmmmKay" );
        }
    }

    CloseConnection();
    if ( IsNetworkInit() )
        HaltNetworkThread();

    HaltGui();
    mainWindow->Remove( &w );
    free( titleList );
    ResumeGui();

    return ret;
}



