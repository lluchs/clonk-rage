/* by Sven2, 2003 */
// startup screen

#include "C4Include.h"
#include "gui/C4LoaderScreen.h"

#ifndef BIG_C4INCLUDE
#include "C4LogBuf.h"
#include "C4Log.h"
#include "game/C4Game.h"
#include "C4Random.h"
#include "c4group/C4GroupSet.h"
#endif

C4LoaderScreen::C4LoaderScreen() : TitleFont(Game.GraphicsResource.FontTitle), LogFont(Game.GraphicsResource.FontTiny)
	{
	// zero fields
	szInfo=NULL;
	fBlackScreen = false;
	}

C4LoaderScreen::~C4LoaderScreen()
	{
	// clear fields
	if (szInfo) delete [] szInfo;
	}

bool C4LoaderScreen::Init(const char *szLoaderSpec)
	{
	// Determine loader specification
	if (!szLoaderSpec || !szLoaderSpec[0]) 
		szLoaderSpec = "Loader*";
	char szLoaderSpecPng[128 + 1 + 4], szLoaderSpecBmp[128 + 1 + 4];
	char szLoaderSpecJpg[128 + 1 + 4], szLoaderSpecJpeg[128 + 1 + 5];
	SCopy(szLoaderSpec, szLoaderSpecPng); DefaultExtension(szLoaderSpecPng, "png");
	SCopy(szLoaderSpec, szLoaderSpecBmp); DefaultExtension(szLoaderSpecBmp, "bmp");
	SCopy(szLoaderSpec, szLoaderSpecJpg); DefaultExtension(szLoaderSpecJpg, "jpg");
	SCopy(szLoaderSpec, szLoaderSpecJpeg); DefaultExtension(szLoaderSpecJpeg, "jpeg");
	int iLoaders=0;
	C4Group *pGroup=NULL,*pChosenGrp=NULL;
	char ChosenFilename[_MAX_PATH+1];
	// query groups of equal priority in set
	while (pGroup=Game.GroupSet.FindGroup(C4GSCnt_Loaders, pGroup, true))
		{
		iLoaders+=SeekLoaderScreens(*pGroup, szLoaderSpecPng, iLoaders, ChosenFilename, &pChosenGrp);
		iLoaders+=SeekLoaderScreens(*pGroup, szLoaderSpecJpeg, iLoaders, ChosenFilename, &pChosenGrp);
		iLoaders+=SeekLoaderScreens(*pGroup, szLoaderSpecJpg, iLoaders, ChosenFilename, &pChosenGrp);
		// lower the chance for any loader other than png
		iLoaders*=2;
		iLoaders+=SeekLoaderScreens(*pGroup, szLoaderSpecBmp, iLoaders, ChosenFilename, &pChosenGrp);
		}
	// nothing found? seek in main gfx grp
	C4Group GfxGrp;
	if (!iLoaders)
		{
		// open it
		GfxGrp.Close();
		if (!GfxGrp.Open(Config.AtExePath(C4CFN_Graphics)))
			{
			LogFatal(FormatString(LoadResStr("IDS_PRC_NOGFXFILE"),C4CFN_Graphics,GfxGrp.GetError()).getData()); 
			return FALSE;
			}
		// seek for png-loaders
		iLoaders=SeekLoaderScreens(GfxGrp, szLoaderSpecPng, iLoaders, ChosenFilename, &pChosenGrp);
		iLoaders+=SeekLoaderScreens(GfxGrp, szLoaderSpecJpg, iLoaders, ChosenFilename, &pChosenGrp);
		iLoaders+=SeekLoaderScreens(GfxGrp, szLoaderSpecJpeg, iLoaders, ChosenFilename, &pChosenGrp);
		iLoaders*=2;
		// seek for bmp-loaders
		iLoaders+=SeekLoaderScreens(GfxGrp, szLoaderSpecBmp, iLoaders, ChosenFilename, &pChosenGrp);
		// Still nothing found: fall back to general loader spec in main graphics group
		if (!iLoaders)
			{
			iLoaders = SeekLoaderScreens(GfxGrp, "Loader*.png", 0, ChosenFilename, &pChosenGrp);
			iLoaders += SeekLoaderScreens(GfxGrp, "Loader*.jpg", iLoaders, ChosenFilename, &pChosenGrp);
			iLoaders += SeekLoaderScreens(GfxGrp, "Loader*.jpeg", iLoaders, ChosenFilename, &pChosenGrp);
			}
		// Not even default loaders available? Fail.
		if (!iLoaders) 
			{
			LogFatal(FormatString("No loaders found for loader specification: %s/%s/%s/%s", szLoaderSpecPng, szLoaderSpecBmp, szLoaderSpecJpg, szLoaderSpecJpeg).getData());
			return FALSE;
			}
		}

	// load loader
	fctBackground.GetFace().SetBackground();
	if (!fctBackground.Load(*pChosenGrp,ChosenFilename, C4FCT_Full,C4FCT_Full,true)) return FALSE;

	// load info
	if (szInfo) { delete [] szInfo; szInfo=NULL; }
	/*size_t iInfoSize;
	if (Game.ScenarioFile.AccessEntry(C4CFN_Info, &iInfoSize))
		{
		szInfo = new char[iInfoSize+1];
		// load info file
		Game.ScenarioFile.Read(szInfo, iInfoSize);
		// terminate buffer
		szInfo[iInfoSize]=0;
		// insert linebreaks
		SReplaceChar(szInfo, 0x0a, '|');
		}*/

	// init fonts
	if (!Game.GraphicsResource.InitFonts())
		return false;

	// initial draw
	C4Facet cgo;
	cgo.Set(Application.DDraw->lpPrimary,0,0,Config.Graphics.ResX,Config.Graphics.ResY); 
	Draw(cgo);

	// done, success!
	return TRUE;
	}

void C4LoaderScreen::SetBlackScreen(bool fIsBlack)
	{
	// enabled/disables drawing of loader screen
	fBlackScreen = fIsBlack;
	// will be updated when drawn next time
	}

int C4LoaderScreen::SeekLoaderScreens(C4Group &rFromGrp, const char *szWildcard, int iLoaderCount, char *szDstName, C4Group **ppDestGrp)
	{
	BOOL fFound;
	int iLocalLoaders=0;
	char Filename[_MAX_PATH+1];
	for (fFound=rFromGrp.FindEntry(szWildcard, Filename); fFound; fFound=rFromGrp.FindNextEntry(szWildcard, Filename))
		{
		// loader found; choose it, if Daniel wants it that way
		++iLocalLoaders;
		if (!SafeRandom(++iLoaderCount))
			{
			// copy group and path
			*ppDestGrp=&rFromGrp;
			SCopy(Filename, szDstName, _MAX_PATH);
			}
		}
	return iLocalLoaders;
	}

void C4LoaderScreen::Draw(C4Facet &cgo, int iProgress, C4LogBuffer *pLog, int Process)
	{
	// simple black screen loader?
	if (fBlackScreen)
		{
		lpDDraw->FillBG();
		return;
		}
	// cgo.X/Y is assumed 0 here...
	// fixed positions for now
	int iHIndent=20;
	int iVIndent=20;
	int iLogBoxHgt=84;
	int iLogBoxMargin=2;
	int iVMargin=5;
	int iProgressBarHgt=15;
	CStdFont &rLogBoxFont=LogFont, &rProgressBarFont=Game.GraphicsResource.FontRegular;
	float fLogBoxFontZoom=1.0f;
	// Background (loader)
	fctBackground.DrawFullScreen(cgo);
	// draw scenario title
	Application.DDraw->StringOut(Game.ScenarioTitle.getData(), TitleFont, 1.0f, cgo.Surface, cgo.Wdt-iHIndent, cgo.Hgt-iVIndent-iLogBoxHgt-iVMargin-iProgressBarHgt-iVMargin-TitleFont.iLineHgt, 0xdddddddd, ARight, false);
	// draw info
	/*if (szInfo)
		Application.DDraw->TextOutDw(szInfo, cgo.Surface, cgo.Wdt/2, cgo.Hgt/2+20);*/
	//
	// draw progress bar
	Application.DDraw->DrawBoxDw(cgo.Surface, iHIndent, cgo.Hgt-iVIndent-iLogBoxHgt-iVMargin-iProgressBarHgt, cgo.Wdt-iHIndent, cgo.Hgt-iVIndent-iLogBoxHgt-iVMargin, 0x4f000000);
	int iProgressBarWdt=cgo.Wdt-iHIndent*2-2;
	if (C4GUI::IsGUIValid())
		{
		C4GUI::GetRes()->fctProgressBar.DrawX(cgo.Surface, iHIndent+1, cgo.Hgt-iVIndent-iLogBoxHgt-iVMargin-iProgressBarHgt+1, iProgressBarWdt*iProgress/100, iProgressBarHgt-2);
		}
	else
		{
		Application.DDraw->DrawBoxDw(cgo.Surface, iHIndent+1, cgo.Hgt-iVIndent-iLogBoxHgt-iVMargin-iProgressBarHgt+1, iHIndent+1+iProgressBarWdt*iProgress/100, cgo.Hgt-iVIndent-iLogBoxHgt-iVMargin-1, 0x4fff0000);
		}
	sprintf(OSTR, "%i%%", iProgress);
	Application.DDraw->StringOut(OSTR, rProgressBarFont, 1.0f, cgo.Surface, cgo.Wdt/2, cgo.Hgt-iVIndent-iLogBoxHgt-iVMargin-rProgressBarFont.iLineHgt/2-iProgressBarHgt/2, 0xffffffff, ACenter, true);
	// draw log box
	if (pLog)
		{
		Application.DDraw->DrawBoxDw(cgo.Surface, iHIndent, cgo.Hgt-iVIndent-iLogBoxHgt, cgo.Wdt-iHIndent, cgo.Hgt-iVIndent, 0x7f000000);
		int iLineHgt=int(fLogBoxFontZoom*rLogBoxFont.iLineHgt); if (!iLineHgt) iLineHgt=5;
		int iLinesVisible = (iLogBoxHgt-2*iLogBoxMargin)/iLineHgt;
		int iX = iHIndent+iLogBoxMargin;
		int iY = cgo.Hgt-iVIndent-iLogBoxHgt+iLogBoxMargin;
		int32_t w,h;
		for (int i = -iLinesVisible; i < 0; ++i)
			{
			const char *szLine = pLog->GetLine(i, NULL, NULL, NULL);
			if (!szLine || !*szLine) continue;
			rLogBoxFont.GetTextExtent(szLine, w,h, true);
			lpDDraw->TextOut(szLine,rLogBoxFont,fLogBoxFontZoom,cgo.Surface,iX,iY);
			iY += h;
			}
		// append process text
		if (Process)
			{
			iY -= h; iX += w;
			lpDDraw->TextOut(FormatString("%i%%", (int) Process).getData(),rLogBoxFont,fLogBoxFontZoom,cgo.Surface,iX,iY);
			}
		}
	}