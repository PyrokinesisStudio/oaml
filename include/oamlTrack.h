#ifndef __OAMLTRACK_H__
#define __OAMLTRACK_H__

class ByteBuffer;
class oamlAudio;

class oamlTrack {
private:
	char name[256];
	int mode;

	int loopCount;
	int condCount;

	int fadeIn;
	int fadeOut;
	int xfadeIn;
	int xfadeOut;

	oamlAudio *loopAudios[256];
	oamlAudio *condAudios[256];
	oamlAudio *introAudio;
	oamlAudio *endAudio;

	oamlAudio *curAudio;
	oamlAudio *tailAudio;
	oamlAudio *fadeAudio;

	int Random(int min, int max);

public:
	oamlTrack();
	~oamlTrack();

	void SetName(const char *trackName) { ASSERT(trackName != NULL); strcpy(name, trackName); }
	void SetMode(int trackMode) { mode = trackMode; }
	void SetFadeIn(int trackFadeIn) { fadeIn = trackFadeIn; }
	void SetFadeOut(int trackFadeOut) { fadeOut = trackFadeOut; }
	void SetXFadeIn(int trackXFadeIn) { xfadeIn = trackXFadeIn; }
	void SetXFadeOut(int trackXFadeOut) { xfadeOut = trackXFadeOut; }

	char *GetName() { return name; }

	void AddAudio(oamlAudio *audio);
	void Play();
	void PlayNext();
	void XFadePlay();

	void Stop();

	bool IsPlaying();

	int Read32();

	void SetCondition(int id, int value);
};

#endif
