import {TagStreamerProvider} from "@/components/helper_components/BackendIntegration.jsx";


export function AppProvider({children}) {

    return (
        <TagStreamerProvider>
            {children}
        </TagStreamerProvider>
    )
}