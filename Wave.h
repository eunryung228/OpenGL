//	Wave & mp3 load and play (Header)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
#pragma once

class CWave {
public:
	WAVEFORMATEX m_WaveFormatEx; 
	int m_nSampleCnt;
	short *m_pSoundData;

	HWAVEOUT m_hPlay;
	HWAVEIN m_hStop;
	WAVEHDR m_WaveHdr;

	CWave() {
		m_pSoundData = NULL;
		m_nSampleCnt = 0;
	}

	~CWave() {
		if(m_pSoundData) delete [] m_pSoundData;
	}

	void Init(int nSampleCnt)
	{
		m_nSampleCnt = nSampleCnt;

		m_pSoundData = new short[m_nSampleCnt];
	}
};

void PlayWave(CWave *pWave); // play 시키면 됨
bool LoadWave(char *FileName, CWave *pWave); // wave file을 load함
bool LoadMp3(char *FileName, CWave *pWave);