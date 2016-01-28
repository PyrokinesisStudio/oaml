#ifndef __OAMLTRACK_H__
#define __OAMLTRACK_H__

class ByteBuffer;
class oamlAudio;

class oamlTrack {
private:
	std::string name;
	int mode;

	int loopCount;
	int condCount;
	int randCount;

	int fadeIn;
	int fadeOut;
	int xfadeIn;
	int xfadeOut;

	int playCondSamples;

	unsigned int tailPos;

	oamlAudio *loopAudios[256];
	oamlAudio *randAudios[256];
	oamlAudio *condAudios[256];
	oamlAudio *introAudio;
	oamlAudio *endAudio;
	oamlAudio *playCondAudio;

	oamlAudio *curAudio;
	oamlAudio *tailAudio;
	oamlAudio *fadeAudio;

	int Random(int min, int max);
	oamlAudio* PickNextAudio();

	void PlayNext();
	void PlayCond(oamlAudio *audio);
	void PlayCondWithMovement(oamlAudio *audio);
	void XFadePlay();

public:
	oamlTrack();
	~oamlTrack();

	void SetName(std::string trackName) { name = trackName; }
	void SetMode(int trackMode) { mode = trackMode; }
	void SetFadeIn(int trackFadeIn) { fadeIn = trackFadeIn; }
	void SetFadeOut(int trackFadeOut) { fadeOut = trackFadeOut; }
	void SetXFadeIn(int trackXFadeIn) { xfadeIn = trackXFadeIn; }
	void SetXFadeOut(int trackXFadeOut) { xfadeOut = trackXFadeOut; }

	const char *GetNameStr() { return name.c_str(); }
	std::string GetName() { return name; }

	void AddAudio(oamlAudio *audio);
	void Play();
	void Stop();

	bool IsPlaying();
	void ShowPlaying();
	void ShowInfo();

	int Mix32(int sample, oamlBase *oaml);

	void SetCondition(int id, int value);
};

#endif
