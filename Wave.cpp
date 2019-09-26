//	Wave & mp3 load and play (Implementation)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr

#include <windows.h>
#include "Wave.h"
#include "LinkedList.h"

extern "C" // C형식으로 들고오겠다
{
	#include "minimp3.h"
}

#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")


//bool SoundPlaying = false;

#define BUFFER_COUNT 8

void CALLBACK waveOutProc(HWAVEOUT hWO, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    MMRESULT mmRes;

    switch(uMsg)
    {
    case MM_WOM_DONE:     
		mmRes = waveOutUnprepareHeader(hWO, (LPWAVEHDR)dwParam2, sizeof(WAVEHDR));
		//SoundPlaying = false;
        break;
    case WIM_DATA:
        break;
    }
}

void PlayWave(CWave *pWave) {
	DWORD mmReturn = 0;
	
	//if(!SoundPlaying)
	{
		WAVEFORMATEX sWaveFormat;
		sWaveFormat = pWave->m_WaveFormatEx;

		MMRESULT mmReturn = waveOutOpen(&pWave->m_hPlay, WAVE_MAPPER, &pWave->m_WaveFormatEx, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);
		if(mmReturn)
		{
			return;
		}
		
		if(!mmReturn)
		{
			//SoundPlaying = true;
		}

		pWave->m_WaveHdr.lpData            = (char *)pWave->m_pSoundData;
		pWave->m_WaveHdr.dwBufferLength    = pWave->m_nSampleCnt;
		pWave->m_WaveHdr.dwFlags           = 0;
		pWave->m_WaveHdr.dwLoops           = 0;
		
		waveOutPrepareHeader(pWave->m_hPlay, &pWave->m_WaveHdr, sizeof(pWave->m_WaveHdr));
		waveOutWrite(pWave->m_hPlay, &pWave->m_WaveHdr, sizeof(pWave->m_WaveHdr));
	}
}

bool LoadMp3(char *FileName, CWave *pWave)
{
	HANDLE hFile, hMap;
	int bytes_left;
	unsigned char *stream_pos;
	unsigned char *inptr;
	mp3_decoder_t mp3;
	mp3_info_t info;
	int byte_count;
	short sample_buffer[MP3_MAX_SAMPLES_PER_FRAME * BUFFER_COUNT];

	hFile = CreateFile(FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    bytes_left = GetFileSize(hFile, NULL) - 128;
    hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    stream_pos = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
    
    if (!stream_pos) {
        return false;
    }

    
    // check for a ID3 tag
    inptr = stream_pos + bytes_left;
    if (((*(unsigned long *)inptr) & 0xFFFFFF) == 0x474154) {
        // Tag information
    }
    
    // set up minimp3 and decode the first frame
    mp3 = mp3_create();

	byte_count = mp3_decode((mp3_decoder_t*)mp3, stream_pos, bytes_left, sample_buffer, &info);

	if(info.channels != 1 && info.channels != 2)
		return false;

	CIpList<short> SoundData;

	int writeShort = 0;
	do{
		byte_count = mp3_decode((mp3_decoder_t*)mp3, stream_pos, bytes_left, sample_buffer, &info);

		stream_pos += byte_count;
        bytes_left -= byte_count;

		if(info.channels == 1)
		{
			int k;
			for(k = 0 ; k < info.audio_bytes/2; k++)
				SoundData.InsertTail(sample_buffer[k]);

			writeShort += info.audio_bytes/2;
		}
		else
		{
			int k;
			for(k = 0 ; k < info.audio_bytes/4; k++)
			{
				SoundData.InsertTail(sample_buffer[k*2]);
			}
			writeShort += info.audio_bytes/4;
			
		}
	}
    while(byte_count);

	pWave->Init(SoundData.GetSize());

	int i = 0;
	CNode4IpList<short>* pHead = SoundData.GetHead();
	while(pHead)
		pWave->m_pSoundData[i++] = SoundData.GetDataForward(&pHead);

	pWave->m_WaveFormatEx.wFormatTag = 1;
	pWave->m_WaveFormatEx.nChannels = 1;//info.channels;
	pWave->m_WaveFormatEx.nSamplesPerSec = info.sample_rate;
	pWave->m_WaveFormatEx.nAvgBytesPerSec = info.sample_rate;
	pWave->m_WaveFormatEx.nBlockAlign = 2;
	pWave->m_WaveFormatEx.wBitsPerSample = 16;
	pWave->m_WaveFormatEx.cbSize = sizeof(WAVEFORMATEX);

	return true;
}

bool LoadWave(char *FileName, CWave *pWave)
{	
	MMCKINFO mmCkInfoRIFF; 
	MMCKINFO mmCkInfoChunk; 
	MMRESULT result; 
	HMMIO hMMIO; 

	long bytesRead; 

	hMMIO = mmioOpen(FileName, NULL, MMIO_READ | MMIO_ALLOCBUF); 

	if (hMMIO == NULL) 
		return false;

	mmCkInfoRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E'); 
	result = mmioDescend(hMMIO, &mmCkInfoRIFF, NULL, MMIO_FINDRIFF); 

	if (result != MMSYSERR_NOERROR) 
		return false;

	mmCkInfoChunk.ckid = mmioFOURCC('f', 'm', 't', ' '); 
	result = mmioDescend(hMMIO, &mmCkInfoChunk, 
	&mmCkInfoRIFF, MMIO_FINDCHUNK); 

	if (result != MMSYSERR_NOERROR) 
		return false;

	bytesRead = mmioRead(hMMIO, (char*)(&(pWave->m_WaveFormatEx)), sizeof(WAVEFORMATEX)); 

	if (bytesRead == -1) 
		return false;

	result = mmioAscend(hMMIO, &mmCkInfoChunk, 0); 

	if (result != MMSYSERR_NOERROR) 
		return false;

	mmCkInfoChunk.ckid = mmioFOURCC('d', 'a', 't', 'a'); 
	result = mmioDescend(hMMIO, &mmCkInfoChunk, &mmCkInfoRIFF, MMIO_FINDCHUNK); 

	if (result != MMSYSERR_NOERROR) 
		return false;

	if(pWave->m_WaveFormatEx.wBitsPerSample != 8 &&
		pWave->m_WaveFormatEx.wBitsPerSample != 16) return false;
	
	if(pWave->m_WaveFormatEx.wBitsPerSample == 8)
	{
		 pWave->Init(mmCkInfoChunk.cksize);
	}
	
	if(pWave->m_WaveFormatEx.wBitsPerSample == 16)
	{
		pWave->Init(mmCkInfoChunk.cksize/2);
	}

	unsigned char *SoundBuf;
	
	if(pWave->m_WaveFormatEx.wBitsPerSample == 16)
	{
		SoundBuf = new unsigned char[pWave->m_nSampleCnt*2];
		bytesRead = mmioRead(hMMIO, (char *)SoundBuf, pWave->m_nSampleCnt*2); 
	}
	else
	{
		SoundBuf = new unsigned char[pWave->m_nSampleCnt];
		bytesRead = mmioRead(hMMIO, (char *)SoundBuf, pWave->m_nSampleCnt); 
	}

	int i;
	if(pWave->m_WaveFormatEx.wBitsPerSample == 16)
	{
		for(i = 0 ; i < pWave->m_nSampleCnt ; i++)
		{
			pWave->m_pSoundData[i] = ((short *)SoundBuf)[i];
		}
	}
	else
	{
		for(i = 0 ; i < pWave->m_nSampleCnt ; i++)
		{
			pWave->m_pSoundData[i] = SoundBuf[i]*32765/255;
		}
		pWave->m_WaveFormatEx.wBitsPerSample = 16;
		pWave->m_WaveFormatEx.nAvgBytesPerSec *= 2;
	}

	delete [] SoundBuf;

	if(pWave->m_WaveFormatEx.nChannels == 2)
	{
		for(i = 0 ; i < pWave->m_nSampleCnt/2 ; i++)
			pWave->m_pSoundData[i] = pWave->m_pSoundData[i*2];

		pWave->m_WaveFormatEx.nChannels = 1;
		pWave->m_nSampleCnt /= 2;
		pWave->m_WaveFormatEx.nAvgBytesPerSec /= 2;
	}

	if (bytesRead == -1) 
	{
		delete [] pWave->m_pSoundData;

		pWave->m_pSoundData = NULL;
		return false;
	}

	mmioClose(hMMIO, 0); 

	pWave->m_WaveFormatEx.nBlockAlign = 2;
	pWave->m_WaveFormatEx.cbSize = sizeof(WAVEFORMATEX);

	return true;
}