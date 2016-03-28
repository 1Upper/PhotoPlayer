//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace PhotoPlayer
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

    private:
        void AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void AppBarButton_SelectFolder(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnTick(Platform::Object ^sender, Platform::Object ^args);

        Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile^>^ _vector;
        Windows::UI::Xaml::DispatcherTimer^ _timer;
        Platform::String^ _photoPath;
        Platform::String^ _accessToken;
        unsigned int _index;
    };
}
