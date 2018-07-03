#ifndef CONFIG_H__
#define CONFIG_H__

struct TUserConfiguration 
{
	TUserConfiguration();
	bool portraitEnabled;
	bool soundEnabled;
	bool jitEnabled;
	bool threadedRendering;
	unsigned int frameSkip;
};

extern TUserConfiguration UserConfiguration;

#endif