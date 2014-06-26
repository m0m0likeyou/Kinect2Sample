// AudioBeam.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
// This source code is licensed under the MIT license. Please see the License in License.txt.
// "This is preliminary software and/or hardware and APIs are preliminary and subject to change."
//

#include "stdafx.h"
#include <Windows.h>
#include <Kinect.h>
#include <iostream>
#include <cstdlib>

#define _USE_MATH_DEFINES
#include <math.h>


template<class Interface>
inline void SafeRelease( Interface *& pInterfaceToRelease )
{
	if( pInterfaceToRelease != NULL ){
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

int _tmain( int argc, _TCHAR* argv[] )
{
	// Sensor
	IKinectSensor* pSensor;
	HRESULT hResult = S_OK;
	hResult = GetDefaultKinectSensor( &pSensor );
	if( FAILED( hResult ) ){
		std::cerr << "Error : GetDefaultKinectSensor" << std::endl;
		return -1;
	}

	hResult = pSensor->Open();
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::Open()" << std::endl;
		return -1;
	}

	// Source
	IAudioSource* pAudioSource;
	hResult = pSensor->get_AudioSource( &pAudioSource );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::get_AudioSource()" << std::endl;
		return -1;
	}

	// Get Audio Beam List
	IAudioBeamList* pAudioBeamList;
	hResult = pAudioSource->get_AudioBeams( &pAudioBeamList );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IAudioSource::get_AudioBeams()" << std::endl;
		return -1;
	}

	// Open Audio Beam
	IAudioBeam* pAudioBeam;
	hResult = pAudioBeamList->OpenAudioBeam( 0, &pAudioBeam );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IAudioBeamList::OpenAudioBeam()" << std::endl;
		return -1;
	}

	// Open Audio Input Stream 
	IStream* pAudioStream;
	hResult = pAudioBeam->OpenInputStream( &pAudioStream );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IAudioBeam::OpenInputStream()" << std::endl;
		return -1;
	}

	static const int samplingFrequency = 16000; // Sampling Frequency 16000[Hz]
	static const int audioBufferLength = samplingFrequency / 1000;

	while( 1 ){
		// Retrieved Audio Data
		FLOAT audioBuffer[audioBufferLength];
		DWORD readSize = 0;

		hResult = pAudioStream->Read( reinterpret_cast<void*>( audioBuffer ), sizeof( audioBuffer ), &readSize );

		if( readSize > 0 ){
			// Get Angle and Confidence
			FLOAT angle = 0.0f;
			FLOAT confidence = 0.0f;

			pAudioBeam->get_BeamAngle( &angle ); // radian [-0.872665f, 0.872665f]
			pAudioBeam->get_BeamAngleConfidence( &confidence ); // confidence [0.0f, 1.0f]

			// Show Result
			// Convert from radian to degree : degree = radian * 180 / Pi
			if( confidence > 0 ){
				std::system( "cls" );
				std::cout << "Angle : " << angle * 180.0f / M_PI << ", Confidence : " << confidence << std::endl;
			}
		}

		// Input Key ( Exit ESC key )
		if( GetKeyState( VK_ESCAPE ) < 0 ){
				break;
		}
	}

	SafeRelease( pAudioStream );
	SafeRelease( pAudioBeam );
	SafeRelease( pAudioBeamList );
	SafeRelease( pAudioSource );
	if( pSensor ){
		pSensor->Close();
	}
	SafeRelease( pSensor );

	return 0;
}
