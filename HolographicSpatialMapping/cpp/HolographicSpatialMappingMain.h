//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

//
// Comment out this preprocessor definition to disable all of the
// sample content.
//
#define DRAW_SAMPLE_CONTENT

#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"
#include "FloorDetector.h"

#include <windows.networking.sockets.h>
#include <windows.storage.streams.h>
#include <sstream>
#include <iostream>

#ifdef DRAW_SAMPLE_CONTENT
#include "Content\SpatialInputHandler.h"
#include "Content\RealtimeSurfaceMeshRenderer.h"
#endif

// Updates, renders, and presents holographic content using Direct3D.
namespace HolographicSpatialMapping
{
    class HolographicSpatialMappingMain : public DX::IDeviceNotify
    {
    public:

		class SampleSpatialAnchorHelper {
		public:
			SampleSpatialAnchorHelper(Windows::Perception::Spatial::SpatialAnchorStore^ anchorStore);
			//IMap<String^, SpatialAnchor^ >^ GetAnchorMap() { return m_anchorMap; };
			Windows::Foundation::Collections::IMap<Platform::String^, Windows::Perception::Spatial::SpatialAnchor^>^  GetAnchorMap() { return m_anchorMap; };
			void LoadFromAnchorStore();
			void ClearAnchorStore();
			bool TrySaveToAnchorStore();
		private:
			Windows::Perception::Spatial::SpatialAnchorStore^ m_anchorStore;
			//std::shared_ptr<Platform::Collections::Map<String^, SpatialAnchor^>> m_anchorMap;
			//Platform::Collections::Map<String^, SpatialAnchor^ >^ m_anchorMap;
			Windows::Foundation::Collections::IMap<Platform::String^, Windows::Perception::Spatial::SpatialAnchor^>^  m_anchorMap;
		};

        HolographicSpatialMappingMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~HolographicSpatialMappingMain();

        // Sets the holographic space. This is our closest analogue to setting a new window
        // for the app.
        void SetHolographicSpace(Windows::Graphics::Holographic::HolographicSpace^ holographicSpace);

        // Starts the holographic frame and updates the content.
        Windows::Graphics::Holographic::HolographicFrame^ Update();

        // Renders holograms, including world-locked content.
        bool Render(Windows::Graphics::Holographic::HolographicFrame^ holographicFrame);

        // Handle saving and loading of app state owned by AppMain.
        void SaveAppState();
        void LoadAppState();

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

        // Handle surface change events.
        void OnSurfacesChanged(Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver^ sender, Platform::Object^ args);

    private:
        // Asynchronously creates resources for new holographic cameras.
        void OnCameraAdded(
            Windows::Graphics::Holographic::HolographicSpace^ sender,
            Windows::Graphics::Holographic::HolographicSpaceCameraAddedEventArgs^ args);

        // Synchronously releases resources for holographic cameras that are no longer
        // attached to the system.
        void OnCameraRemoved(
            Windows::Graphics::Holographic::HolographicSpace^ sender,
            Windows::Graphics::Holographic::HolographicSpaceCameraRemovedEventArgs^ args);
        
        // Used to prevent the device from deactivating positional tracking, which is 
        // necessary to continue to receive spatial mapping data.
        void OnPositionalTrackingDeactivating(
            Windows::Perception::Spatial::SpatialLocator^ sender,
            Windows::Perception::Spatial::SpatialLocatorPositionalTrackingDeactivatingEventArgs^ args);
		void OnLocatabilityChanged(
			Windows::Perception::Spatial::SpatialLocator^ sender,
			Platform::Object^ args);
        // Clears event registration state. Used when changing to a new HolographicSpace
        // and when tearing down AppMain.
        void UnregisterHolographicEventHandlers();

#ifdef DRAW_SAMPLE_CONTENT
        // Listens for the Pressed spatial input event.
        std::shared_ptr<SpatialInputHandler>                                m_spatialInputHandler;

        // A data handler for surface meshes.
        std::unique_ptr<WindowsHolographicCodeSamples::RealtimeSurfaceMeshRenderer> m_meshRenderer;
#endif

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                                m_deviceResources;

        // Render loop timer.
        DX::StepTimer                                                       m_timer;

        // Represents the holographic space around the user.
        Windows::Graphics::Holographic::HolographicSpace^                   m_holographicSpace;

        // SpatialLocator that is attached to the primary camera.
        Windows::Perception::Spatial::SpatialLocator^                       m_locator;
		std::shared_ptr<SampleSpatialAnchorHelper>							m_spatialAnchorHelper;
		Windows::Perception::Spatial::SpatialAnchor							^m_baseAnchor, ^m_nextAnchor, ^m_anchor;

        // A reference frame attached to the holographic camera.
        Windows::Perception::Spatial::SpatialLocatorAttachedFrameOfReference^ m_referenceFrame;
		Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ m_stationaryReferenceFrame;

        // Event registration tokens.
        Windows::Foundation::EventRegistrationToken                         m_cameraAddedToken;
        Windows::Foundation::EventRegistrationToken                         m_cameraRemovedToken;
        Windows::Foundation::EventRegistrationToken                         m_positionalTrackingDeactivatingToken;
        Windows::Foundation::EventRegistrationToken                         m_surfacesChangedToken;
		Windows::Foundation::EventRegistrationToken                         m_locatabilityChangedToken;
        // Indicates whether access to spatial mapping data has been granted.
        bool                                                                m_surfaceAccessAllowed = false;

        // Indicates whether the surface observer initialization process was started.
        bool                                                                m_spatialPerceptionAccessRequested = false;

        // Obtains spatial mapping data from the device in real time.
        Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver^     m_surfaceObserver;
        Windows::Perception::Spatial::Surfaces::SpatialSurfaceMeshOptions^  m_surfaceMeshOptions;

		Windows::Networking::Sockets::StreamSocketListener ^            listener, ^listener2, ^listener3;
		Windows::Foundation::TypedEventHandler<Windows::Networking::Sockets::StreamSocketListener^, Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs^>^  OnConnection;
		Windows::Foundation::TypedEventHandler<Windows::Networking::Sockets::StreamSocketListener^, Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs^>^  OnConnection2, ^  OnConnection3;
		Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs^ OnConnectionEvent;

		// Determines the rendering mode.
		bool                                                                m_drawWireframe = true;


		bool																m_renderAndSend = false;
		bool																m_positionLost = false;
		bool																m_recovering = false;
		bool																m_depthReceived = false;
		void																LoadAnchorStore();
		int																	m_spatialId = 0;
		Platform::String^																m_newKey;

    };
}
