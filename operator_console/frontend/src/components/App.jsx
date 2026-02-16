import {AppProvider} from "@/components/AppContext.jsx";
import {socket} from "@/other/socket.js";
import {toast} from "sonner";
import {Toaster} from "@/components/ui/sonner.jsx";
import {MainPage} from "@/components/main_app/MainPage.jsx";
import {ThemeProvider} from "next-themes";
import {SidebarTrigger} from "@/components/ui/sidebar.jsx";
import {BrowserRouter} from "react-router-dom";

function App() {

    //display websocket connection error messages.
    socket.onError((e) => {
        try {
            let message = "WebSocket connection to '"+e.currentTarget.url +"' failed."
            toast.error(message);
        } catch (e) {
            let error = e;
        }
    })

  return (
      <ThemeProvider attribute="class" defaultTheme="dark">
      <AppProvider>
          <Toaster
              theme="dark"
              toastOptions={{
                  classNames: {
                      error: '!bg-red-800 border-black',
                      success: '',
                      warning: '!bg-yellow-600 border-black',
                      info: '',
                  },
              }}
          />
          <BrowserRouter>
              <MainPage/>
          </BrowserRouter>
      </AppProvider>
      </ThemeProvider>
  )
}

export default App
