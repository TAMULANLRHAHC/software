import {
    Sidebar,
    SidebarContent,
    SidebarGroup,
    SidebarGroupContent,
    SidebarGroupLabel,
    SidebarMenu,
    SidebarMenuButton,
    SidebarMenuItem,
    SidebarMenuSub,
    SidebarMenuSubItem,
    SidebarMenuSubButton, SidebarRail, SidebarTrigger, SidebarHeader,
} from "@/components/ui/sidebar"
import {ScrollArea} from "@/components/ui/scroll-area.jsx";
import {
    Breadcrumb,
    BreadcrumbItem,
    BreadcrumbLink, BreadcrumbList,
    BreadcrumbPage,
    BreadcrumbSeparator
} from "@/components/ui/breadcrumb.jsx";
import {Separator} from "@radix-ui/react-separator";
import {Slash, Airplay, GalleryVerticalEnd} from "lucide-react"
import {Link} from "react-router-dom";
import * as React from "react";



export function AppSidebar({sidebarContent}) {
    return (
        <Sidebar>
            <SidebarHeader>
                <SidebarMenu>
                    <SidebarMenuItem>
                        <SidebarMenuButton size="lg" asChild className="border-zinc-800 border-1">
                            <div>
                                <div className=" text-sidebar-primary-foreground flex aspect-square size-8 items-center justify-center rounded-lg">
                                    <Airplay className="size-4" />
                                </div>
                                <div className="flex flex-col gap-0.5 leading-none">
                                    <span className="font-medium">HAOS</span>
                                    <span className="font-thin text-[12px]">Hydraulic Arm Operation Software</span>
                                </div>
                            </div>
                        </SidebarMenuButton>
                    </SidebarMenuItem>
                </SidebarMenu>
            </SidebarHeader>
            <ScrollArea className="h-full">
                <SidebarContent className="p-4">
                    {sidebarContent.map((group) => (
                        <SidebarGroup key={group.label} className="bg-zinc-950 border-zinc-800 border-1 rounded-sm">
                            <SidebarGroupLabel>{group.label}</SidebarGroupLabel>
                            <SidebarGroupContent>
                                <SidebarMenu>
                                    {group.items.map((item) => (
                                        <SidebarMenuItem key={item.title}>
                                            <SidebarMenuButton asChild>
                                                <Link to={item.url}>
                                                    {item.icon}
                                                    <span>{item.title}</span>
                                                </Link>
                                            </SidebarMenuButton>
                                            {item.subItems && (
                                                <SidebarMenuSub>
                                                    {item.subItems.map((subItem) => (
                                                        <SidebarMenuSubItem key={subItem.title}>
                                                            <SidebarMenuSubButton asChild>
                                                                <a href={subItem.url}>
                                                                    <span>{subItem.title}</span>
                                                                </a>
                                                            </SidebarMenuSubButton>
                                                        </SidebarMenuSubItem>
                                                    ))}
                                                </SidebarMenuSub>
                                            )}
                                        </SidebarMenuItem>
                                    ))}
                                </SidebarMenu>
                            </SidebarGroupContent>
                        </SidebarGroup>
                    ))}
                </SidebarContent>
            </ScrollArea>
            <SidebarRail/>
        </Sidebar>
    )
}

export function AppSidebarInsertPage(props) {

    let component = props.component
    let url = props.url

    //Build Breadcrum Trail
    let breadcrum_trail = []
    let spliturl = url.split("/")
    for (let i = 0; i < spliturl.length; i++) {
        //Empty
        if (spliturl[i] === "") {
            continue;
        }
        //Breadcrum link - items except for the last item
        else if ((i+1) < spliturl.length) {
            breadcrum_trail.push(
                <BreadcrumbItem className="hidden md:block" key={i+1}>
                    <BreadcrumbLink key={i}>
                        {spliturl[i]}
                    </BreadcrumbLink>
                </BreadcrumbItem>

            );
            //Add Separator
            breadcrum_trail.push(
                <BreadcrumbSeparator key={i}>
                    <Slash />
                </BreadcrumbSeparator>
            )
        }
        //Breadcrum page - last item
        else if ((i+1) === spliturl.length) {
            breadcrum_trail.push(
                <BreadcrumbItem key={i+1}>
                    <BreadcrumbPage key={i}>
                        {spliturl[i]}
                    </BreadcrumbPage>
                </BreadcrumbItem>
            );
        }
    }

    //BELOW I have a small fix applied to the max height the component can take up.
    //It is the total height minus the header height. 16 in tcss is 64px.
    return (
        <div className="w-full h-full overflow-y-clip">
            <header className="flex h-16 shrink-0 items-center gap-2 border-b bg-zinc-950">
                <div className="flex items-center gap-2 px-3">
                    <SidebarTrigger/>
                    <Separator orientation="vertical" className="mr-2 h-4"/>
                    <Breadcrumb>
                        <BreadcrumbList>
                            {breadcrum_trail}
                        </BreadcrumbList>
                    </Breadcrumb>
                </div>
            </header>
            <div className=" top-0 h-[calc(100vh-64px)]">
                {component}
            </div>
        </div>
    );
}

