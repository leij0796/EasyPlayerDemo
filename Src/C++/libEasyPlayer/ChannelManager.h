/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Sword@easydarwin.org
*/
#pragma once

#include <queue>

#include "libEasyPlayerAPI.h"
#include "EasyRTSPClient\EasyRTSPClientAPI.h"
#include "FFDecoder\FFDecoderAPI.h"
#include "D3DRender\D3DRenderAPI.h"
#include "SoundPlayer.h"
#include "ssqueue.h"
#pragma comment(lib, "EasyRTSPClient/libEasyRTSPClient.lib")
#pragma comment(lib, "FFDecoder/FFDecoder.lib")
#pragma comment(lib, "D3DRender/D3DRender.lib")

#include "IntelHardCodec/IntelHardCodec_Interface.h"
#pragma comment(lib, "IntelHardCodec/IntelHardCodec.lib")

#include "libEasyAACEncoder/EasyAACEncoderAPI.h"
#pragma comment(lib, "libEasyAACEncoder/libEasyAACEncoder.lib")

// 增加MP4box库的支持 [9/20/2016 dingshuai]
//gpac write mp4 file
#include "EasyMP4Writer/EasyMP4Writer.h"

#ifdef __cplusplus 
extern "C"
{
#endif
#include "mp4creator\libmp4creator.h"
//#pragma comment(lib, "libMp4Creator.lib")

#include "libavutil/avstring.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"

#ifdef __cplusplus 
	}
#endif

#define		MAX_CHANNEL_NUM		64		//可以解码显示的最大通道数
#define		MAX_DECODER_NUM		5		//一个播放线程中最大解码器个数
#define		MAX_YUV_FRAME_NUM	3		//解码后的最大YUV帧数
#define		MAX_CACHE_FRAME		30		//最大帧缓存,超过该值将只播放I帧
//#define		MAX_AVQUEUE_SIZE	(1024*1024)	//队列大小
#define		MAX_AVQUEUE_SIZE	(1920*1080*2)	//队列大小

typedef struct __CODEC_T
{
	//Video Codec
	unsigned int	vidCodec;
	int				width;
	int				height;
	int				fps;
	float			bitrate;

	//Audio Codec
	unsigned int	audCodec;
	int				samplerate;
	int				channels;
}CODEC_T;
typedef struct __DECODER_OBJ
{
	bool bHardDecode;
	CODEC_T			codec;
	FFD_HANDLE		ffDecoder;
	LPIntelHardDecoder pIntelDecoder;
	int				yuv_size;
}DECODER_OBJ;


typedef struct __THREAD_OBJ
{
	int			flag;
	HANDLE		hThread;
}THREAD_OBJ;

typedef struct _YUV_FRAME_INFO			//YUV信息
{
	MEDIA_FRAME_INFO	frameinfo;
	char	*pYuvBuf;
	int		Yuvsize;
}YUV_FRAME_INFO;

typedef struct tagRECORD_FRAME_INFO
{
	tagRECORD_FRAME_INFO() : pDataBuffer(NULL)
		, nBufSize(0), nTimestamp(0),bIsVideo(FALSE),nID(-1)
	{

		//音视频参数
		memset(&mediaInfo, 0x00, sizeof(MEDIA_FRAME_INFO));
	}
	tagRECORD_FRAME_INFO& operator=(const tagRECORD_FRAME_INFO& src)
	{
		pDataBuffer	 = src.pDataBuffer;
		nBufSize     = src.nBufSize;
		nTimestamp   = src.nTimestamp;
		bIsVideo	=	src.bIsVideo;
		nID			=	src.nID;
		//视频参数
		memcpy(&mediaInfo, &src.mediaInfo, sizeof(MEDIA_FRAME_INFO) );
	}
	//结构数据信息参数
	unsigned char* pDataBuffer;
	int nBufSize;
	long nTimestamp;//时间戳
	bool bIsVideo;//标识是否为视频
	bool bKeyFrame;//标识是否为关键帧(对视频有效)
	int nID;//文件ID
	// 音视频信息
	MEDIA_FRAME_INFO mediaInfo;
}RECORD_FRAME_INFO;

typedef std::queue<RECORD_FRAME_INFO*> QueueFrame;

typedef struct __PLAY_THREAD_OBJ
{
	THREAD_OBJ		decodeThread;		//解码线程
	THREAD_OBJ		displayThread;			//显示线程
	THREAD_OBJ		recordThread;			//录像线程
	// 
	Easy_Handle		nvsHandle;
	HWND			hWnd;				//显示视频的窗口句柄
	int				channelId;			//通道号
	int				showStatisticalInfo;//显示统计信息
	int				showOSD;//显示OSD信息
	//OSD自定义叠加
	EASY_PALYER_OSD osd;
	//缓存帧信息
	MEDIA_INFO frameinfo;

	int				frameCache;		//帧缓存(用于调整流畅度),由上层应用设置
	int				initQueue;		//初始化队列标识
	SS_QUEUE_OBJ_T	*pAVQueue;		//接收rtsp的帧队列
	int				frameQueue;		//队列中的帧数
	int				findKeyframe;	//是否需要查找关键帧标识
	int				decodeYuvIdx;

	DWORD			dwLosspacketTime;	//丢包时间
	DWORD			dwDisconnectTime;	//断线时间

	DECODER_OBJ		decoderObj[MAX_DECODER_NUM];
	D3D_HANDLE		d3dHandle;		//显示句柄
	D3D_SUPPORT_FORMAT	renderFormat;	//显示格式
	int				ShownToScale;		//按比例显示
	int				decodeKeyFrameOnly;	//仅解码显示关键帧

	unsigned int	rtpTimestamp;
	LARGE_INTEGER	cpuFreq;		//cpu频率
	_LARGE_INTEGER	lastRenderTime;	//最后显示时间

	int				yuvFrameNo;		//当前显示的yuv帧号
	YUV_FRAME_INFO	yuvFrame[MAX_YUV_FRAME_NUM];
	CRITICAL_SECTION	crit;
	bool			resetD3d;		//是否需要重建d3dRender
	RECT			rcSrcRender;
	D3D9_LINE		d3d9Line;
	
	char			url[MAX_PATH];//播放的URL路径
	char			manuRecordingPath[MAX_PATH];//录像存放路径
	char			strScreenCapturePath[MAX_PATH];//抓图存放路径
	//char			manuRecordingFile[MAX_PATH];
	int				manuRecording;	//=1 开启录制
	int				manuScreenshot;//=1 开启抓图
	//MP4Creator写MP4
	MP4C_Handler	mp4cHandle;
	//MP4Box Writer
	EasyMP4Writer* m_pMP4Writer;
	int				vidFrameNum;
	MediaSourceCallBack pCallback;
	void			*pUserPtr;

	//录像缓存帧队列
	int				initRecQueue;		//初始化队列标识
	SS_QUEUE_OBJ_T	*pRecAVQueue;		//接收rtsp的帧队列
	CRITICAL_SECTION	critRecQueue;	

}PLAY_THREAD_OBJ;


//音频播放线程
typedef struct __AUDIO_PLAY_THREAD_OBJ
{
	int				channelId;		//当前播放通道号

	unsigned int	samplerate;	//采样率
	unsigned int	audiochannels;	//声道
	unsigned int	bitpersample;

	//CWaveOut		*pWaveOut;
	CSoundPlayer	*pSoundPlayer;
}AUDIO_PLAY_THREAD_OBJ;

//抓图线程结构体
typedef struct tagPhotoShotThreadInfo
{
	unsigned char* pYuvBuf;
	int width;
	int height;
	char strPath[512];
	int renderFormat;
}PhotoShotThreadInfo;

class CChannelManager
{
public:
	CChannelManager(void);
	virtual ~CChannelManager(void);

	int		Initial();

	//OpenStream 返回一个可用的通道ID
	int		OpenStream(const char *url, HWND hWnd, RENDER_FORMAT renderFormat, int _rtpovertcp, const char *username, const char *password, MediaSourceCallBack callback=NULL, void *userPtr=NULL, bool bHardDecode=true,
		char* startTime = NULL, char* endTime = NULL, float fScale = 1.0f);
	void 	CloseStream(int channelId);
	int		ShowStatisticalInfo(int channelId, int _show);
	int		ShowOSD(int channelId, int _show, EASY_PALYER_OSD osd);
	int		SetFrameCache(int channelId, int _cache);
	int		SetShownToScale(int channelId, int ShownToScale);
	int		SetDecodeType(int channelId, int _decodeKeyframeOnly);
	int		SetRenderRect(int channelId, LPRECT lpSrcRect);
	int		DrawLine(int channelId, LPRECT lpRect);
	int		SetDragStartPoint(int channelId, POINT pt);
	int		SetDragEndPoint(int channelId, POINT pt);
	int		ResetDragPoint(int channelId);
	int		GetMediaInfo(int channelId, MEDIA_INFO& mediaInfo);
	//设置回放速度
	int		SetPlaybackSpeed(int channelId, float fSpeed);
	/* 跳转播放时间 */
	int		PlaybackSeek(int channelId, const char *playTime);
	/* 暂停播放 */
	int		PlaybackPause(int channelId);
	/* 恢复播放 */
	int		PlaybackResume(int channelId);


	//同一时间只支持一路声音播放
	int		PlaySound(int channelId);
	int		StopSound();

	int		StartManuRecording(int channelId);
	int		StopManuRecording(int channelId);

	int		SetManuRecordPath(int channelId, const char* recordPath);
	int		SetManuPicShotPath(int channelId, const char* shotPath);

	int		StartManuPicShot(int channelId);//pThread->manuScreenshot
	int		StopManuPicShot(int channelId);

	static LPTHREAD_START_ROUTINE __stdcall _lpDecodeThread( LPVOID _pParam );
	static LPTHREAD_START_ROUTINE __stdcall _lpDisplayThread( LPVOID _pParam );
	static LPTHREAD_START_ROUTINE __stdcall _lpPhotoShotThread( LPVOID _pParam );
	static LPTHREAD_START_ROUTINE __stdcall _lpRecordThread( LPVOID _pParam );


	int		ProcessData(int _chid, int mediatype, char *pbuf, EASY_FRAME_INFO *frameinfo);
	static bool ConvertImage(AVPixelFormat eInFormat, int iInWidth, int iInHeight, void* ptInData,
		AVPixelFormat eOutFormat, int iOutWidth, int iOutHeight, unsigned char** pOutData);

protected:
	bool				GetD3DSupportFormat();			//获取D3D支持的格式
	void	CreatePlayThread(PLAY_THREAD_OBJ	*_pPlayThread);
	void	ClosePlayThread(PLAY_THREAD_OBJ		*_pPlayThread);
	void CreateRecordThread(PLAY_THREAD_OBJ	*_pPlayThread);
	void CloseRecordThread(PLAY_THREAD_OBJ	*_pPlayThread);

	int		SetAudioParams(unsigned int _channel, unsigned int _samplerate, unsigned int _bitpersample);
	void	ClearAllSoundData();
	void	Release();	
	
private:
	PLAY_THREAD_OBJ			*pRealtimePlayThread;		//实时播放线程
	AUDIO_PLAY_THREAD_OBJ	*pAudioPlayThread;			//音频播放线程
	CRITICAL_SECTION		crit;
	D3D_ADAPTER_T		d3dAdapter;
	bool m_bIFrameArrive;
};
extern CChannelManager	*pChannelManager;
