#ifndef GAME_CLIENT_COMPONENTS_RCLIENT_VOICE_H
#define GAME_CLIENT_COMPONENTS_RCLIENT_VOICE_H

#include <base/system.h>

#include <SDL_audio.h>

#include <engine/shared/protocol.h>

#include <array>
#include <cstdint>

class CGameClient;
class IClient;
class IConsole;
struct OpusDecoder;
struct OpusEncoder;

class CRClientVoice
{
	struct SVoicePeer
	{
		static constexpr int MAX_FRAMES = 8;
		struct SVoiceFrame
		{
			int16_t m_aPcm[960] = {};
			int m_Samples = 0;
			float m_LeftGain = 1.0f;
			float m_RightGain = 1.0f;
		};

		OpusDecoder *m_pDecoder = nullptr;
		uint16_t m_LastSeq = 0;
		bool m_HasSeq = false;
		SVoiceFrame m_aFrames[MAX_FRAMES] = {};
		int m_FrameHead = 0;
		int m_FrameTail = 0;
		int m_FrameCount = 0;
		int m_FrameReadPos = 0;
	};

	CGameClient *m_pGameClient = nullptr;
	IClient *m_pClient = nullptr;
	IConsole *m_pConsole = nullptr;

	NETSOCKET m_Socket = nullptr;
	NETADDR m_ServerAddr = NETADDR_ZEROED;
	bool m_ServerAddrValid = false;
	char m_aServerAddrStr[128] = {0};

	SDL_AudioDeviceID m_CaptureDevice = 0;
	SDL_AudioDeviceID m_OutputDevice = 0;
	SDL_AudioSpec m_CaptureSpec = {};
	SDL_AudioSpec m_OutputSpec = {};
	char m_aInputDeviceName[128] = {0};
	char m_aOutputDeviceName[128] = {0};
	bool m_OutputStereo = true;
	bool m_LogDeviceChange = false;
	float m_HpfPrevIn = 0.0f;
	float m_HpfPrevOut = 0.0f;
	float m_CompEnv = 0.0f;
	int m_OutputChannels = 0;

	OpusEncoder *m_pEncoder = nullptr;
	std::array<SVoicePeer, MAX_CLIENTS> m_aPeers = {};
	std::array<int64_t, MAX_CLIENTS> m_aLastHeard = {};

	bool m_PttActive = false;
	uint16_t m_Sequence = 0;
	uint32_t m_ContextHash = 0;
	int64_t m_LastKeepalive = 0;

	bool EnsureSocket();
	bool EnsureAudio();
	void Shutdown();
	void UpdateServerAddr();
	void UpdateContext();
	void ProcessCapture();
	void ProcessIncoming();
	void PushPeerFrame(int PeerId, const int16_t *pPcm, int Samples, float LeftGain, float RightGain);
	void MixAudio(int16_t *pOut, int Samples, int OutputChannels);
	void ClearPeerFrames();
	static void SDLAudioCallback(void *pUserData, Uint8 *pStream, int Len);
	const char *FindDeviceName(bool Capture, const char *pDesired) const;

public:
	void Init(CGameClient *pGameClient, IClient *pClient, IConsole *pConsole);
	void OnRender();
	void SetPttActive(bool Active);
	void ListDevices();
	bool IsVoiceActive(int ClientId) const;
};

#endif // GAME_CLIENT_COMPONENTS_RCLIENT_VOICE_H
