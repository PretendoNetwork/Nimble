#ifndef SCREEN__H
#define SCREEN__H

class WiiUScreen
{
public:
	WiiUScreen();
	~WiiUScreen();
	
	void DrawText(const char *text);
	void DrawTextf(const char *text, ...);
	void DrawTextLine(int line, const char *text);
	void FlushBuffers();

	void StartFrame();
	void EndFrame();

	int BufferSizeTV;		// width*height*4
	int BufferSizeDRC;		// width*height*4

	void *TV_Buffer;
	void *DRC_Buffer;

	int Y_Position;

};

#endif