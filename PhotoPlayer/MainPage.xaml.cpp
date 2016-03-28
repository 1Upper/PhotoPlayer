//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace PhotoPlayer;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::AccessCache;
// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();

    _timer = ref new DispatcherTimer();
    TimeSpan ts;
    ts.Duration = 25000000;
    _timer->Interval = ts;
    _timer->Tick += ref new Windows::Foundation::EventHandler<Platform::Object ^>(this, &PhotoPlayer::MainPage::OnTick);
}


void PhotoPlayer::MainPage::AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    _timer->Stop();

    FileOpenPicker^ openPicker = ref new Windows::Storage::Pickers::FileOpenPicker();
    openPicker->ViewMode = PickerViewMode::Thumbnail;
    openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;

    // Users expect to have a filtered view of their folders depending on the scenario.
    // For example, when choosing a documents folder, restrict the filetypes to documents for your application.
    //fileOpenPicker->FileTypeFilter->ReplaceAll(".png", ".jpg", ".jpeg" });
    openPicker->FileTypeFilter->Append(".jpg");
    openPicker->FileTypeFilter->Append(".jpeg");
    openPicker->FileTypeFilter->Append(".png");

    //create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
    //{
    //    if (file)
    //    {
    //        create_task(file->OpenReadAsync())
    //            .then([this](IRandomAccessStreamWithContentType^ imgStream)
    //        {
    //            auto bitmap = ref new BitmapImage();
    //            bitmap->SetSource(imgStream);
    //            ImageDisplay->Source = bitmap;
    //        });
    //    }
    //});

    create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file)
        {
            auto window = Window::Current;

            std::thread([this, file, window]
            {
                try
                {
                    auto streamTask = create_task(file->OpenReadAsync());
                    streamTask.wait();

                    auto imageStream = streamTask.get();

                    window->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
                        ref new DispatchedHandler([this, imageStream]
                    {
                        auto bitmap = ref new BitmapImage();
                        bitmap->SetSource(imageStream);
                        ImageDisplay->Source = bitmap;
                    }));

                    auto folderTask = create_task(StorageFolder::GetFolderFromPathAsync(file->Path));
                    folderTask.get();

                    auto folder = folderTask.get();

                    auto getFilesTask = create_task(folder->GetFilesAsync());
                    getFilesTask.wait();

                    _vector = getFilesTask.get();
                }
                catch (Exception^ e)
                {
                    auto messageDialog = ref new MessageDialog(e->Message, "Error");
                    create_task(messageDialog->ShowAsync());
                }
            }).detach();
        }
    });
}

void PhotoPlayer::MainPage::OnTick(Platform::Object ^sender, Platform::Object ^args)
{
    if (_index < _vector->Size)
    {
        StorageFile^ file = _vector->GetAt(_index);

        if (file)
        {
            create_task(file->OpenReadAsync())
                .then([this](IRandomAccessStreamWithContentType^ imgStream)
            {
                auto bitmap = ref new BitmapImage();
                bitmap->SetSource(imgStream);
                ImageDisplay->Source = bitmap;
            });
        }

        _index++;
    }
    else
    {
        _index = 0;
        _timer->Stop();
    }
}

void PhotoPlayer::MainPage::AppBarButton_SelectFolder(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    _timer->Stop();

    FolderPicker^ folderPicker = ref new FolderPicker();
    folderPicker->FileTypeFilter->Append(".jpg");
    folderPicker->FileTypeFilter->Append(".jpeg");
    folderPicker->FileTypeFilter->Append(".png");

    create_task(folderPicker->PickSingleFolderAsync()).then([this](StorageFolder^ folder)
    {
        if (folder)
        {
            auto window = Window::Current;

            std::thread([this, folder, window]
            {
                try
                {
                    _accessToken = StorageApplicationPermissions::FutureAccessList->Add(folder);
                    
                    auto getFilesTask = create_task(folder->GetFilesAsync());
                    getFilesTask.wait();

                    _vector = getFilesTask.get();
                    _index = 0;

                }
                catch (Exception^ e)
                {
                    auto messageDialog = ref new MessageDialog(e->Message, "Error");
                    create_task(messageDialog->ShowAsync());
                }
            }).detach();

            _timer->Start();
        }
    });
}
