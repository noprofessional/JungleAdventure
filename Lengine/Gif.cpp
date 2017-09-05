#include "Gif.h"
#include "GIF/gif_lib.h"
#include "GIF/getarg.h"
#include "FatalError.h"


namespace Lengine {
	void loadGif(const char* filePath, ucharSheet& out, int& width, int&height) {

		GifFileType* gifFile = nullptr;
		ColorMapObject* colorMap = nullptr;
		int SHeight = 0;
		int SWidth = 0;
		unsigned char transColor = 0;

		//clear all out data
		for (int i = 0;i < out.size();i++) {
			out[i].clear();
		}
		out.clear();

		//read screen information
		int error=0;
		gifFile = DGifOpenFileName(filePath, &error);
		if (gifFile == nullptr) {
			fatalError(GifErrorString(error));
			return;
		}
		SHeight = gifFile->SHeight;
		SWidth = gifFile->SWidth;

		//read all the rest image data
		if (DGifSlurp(gifFile) == GIF_ERROR) {
			fatalError(GifErrorString(gifFile->Error));
			return;
		}

		//resize frame Count
		out.resize(gifFile->ImageCount);

		//convert raw data to RGBA8 unsigned char data
		int SSize = SWidth*SHeight * 4;
		for (int i = 0;i < gifFile->ImageCount;i++) {

			//reference for easy use
			uCharVec& currentFrame = out[i];

			//resize frame size
			currentFrame.resize(SSize);
			//if (i>0) {	//there is already one array in the out buffer
			//	for (int j = 0;j < SSize;j++) {
			//		out[i][j] = out[i-1][j];	//copy the last one to this one
			//	}
			//}

			//raw image data of every frame
			GifImageDesc& CurrentImageDesc = gifFile->SavedImages[i].ImageDesc;
			int Left = CurrentImageDesc.Left;
			int Top = CurrentImageDesc.Top;
			int Height = CurrentImageDesc.Height;
			int Width = CurrentImageDesc.Width;
			
			//if local color map exist use it 
			//else use global map
			colorMap = (CurrentImageDesc.ColorMap ? CurrentImageDesc.ColorMap: gifFile->SColorMap);

			//get Graphic Control Extension Transparent bit data
			ExtensionBlock & LastExtensionBlock =
				gifFile->SavedImages[i].ExtensionBlocks[gifFile->SavedImages[i].ExtensionBlockCount - 1];
			if (LastExtensionBlock.Function = GRAPHICS_EXT_FUNC_CODE)
			{
				transColor = LastExtensionBlock.Bytes[LastExtensionBlock.ByteCount - 1];
			}

			//set image array every pixel's color byte
			for (int row = Top;row < Top+Height;row++) {
				for (int col = Left;col < Left+Width;col++) {

					//alias for easy use
					int DataIndex = (row-Top)*Width + col-Left;
					int colorIndex = 4 * (row*SWidth + col);

					//get raw data
					GifByteType temp = gifFile->SavedImages[i].RasterBits[DataIndex];
					

					//use color map get decode color
					GifColorType* colorEntries = &colorMap->Colors[temp];

					
					if (transColor != -1 && temp == transColor) {
						//if is transparent change alpha to 0 then move to next pixel
						currentFrame[colorIndex + 3] = 0x00;
						continue;
					}
					else {
						currentFrame[colorIndex + 0] = colorEntries->Red;
						currentFrame[colorIndex + 1] = colorEntries->Green;
						currentFrame[colorIndex + 2] = colorEntries->Blue;
						currentFrame[colorIndex + 3] = 0xff;
					}
				}
			}
		}

		//set other parameter
		width = SWidth;
		height = SHeight;

		//close the file
		int errorCode;
		DGifCloseFile(gifFile, &errorCode);
		if (errorCode != 0) {
			fatalError(GifErrorString(error));
			return;
		}
		//done
		return;
	}
}