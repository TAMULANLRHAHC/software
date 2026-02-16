import {createContext, useEffect, useState} from "react";
import {socket} from "@/other/socket.js";

//collects tag data from server to use through out the client
//data is stored as a context and this provider provides it through out the program
export const TagStreamer = createContext()
export function TagStreamerProvider({ children }) {

    //default for the dictionary of tags to request data from
    const [tagSubscribeDictionary, setTagSubscribeDictionary] = useState({})
    //default for the recieved data dictionary
    const [tagData, setTagData] = useState({})

    //add tag to the subscribe dictionary
    function subscribeTag(tag_path) {
        setTagSubscribeDictionary((prevState) => {
            if (!(tag_path in prevState)) {
                // Add new key
                return { ...prevState, [tag_path]: 1 };
            } else {
                // Increment listener count
                return { ...prevState, [tag_path]: prevState[tag_path] + 1 };
            }
        });
    }

    //remove from the subscribe list
    function unsubscribeTag(tag_path) {

        setTagSubscribeDictionary((prevState) => {
            if (tag_path in prevState) {
                const listeners = prevState[tag_path];
                if (listeners === 1) {
                    // Remove the key
                    const { [tag_path]: _, ...rest } = prevState; // Exclude the key
                    return rest;
                } else {
                    // Decrement listener count
                    return { ...prevState, [tag_path]: listeners - 1 };
                }
            }

            return prevState;
        });
    }

    //Send new subscribe list to server
    useEffect(() => {
        //build list from keys in dictionary
        const tag_streamer_subscription_list = Object.keys(tagSubscribeDictionary);

        socket.emit("set_tag_streamer_subscription_list", {tag_streamer_subscription_list: tag_streamer_subscription_list})
    }, [tagSubscribeDictionary]);

    //Recieve tag data from server
    useEffect(() => {
        socket.on("tag_streamer_tag_data", (data)=>{
            //set data
            setTagData(data)
        })
    }, []);

    return (
        <TagStreamer.Provider value={{tagData, subscribeTag, unsubscribeTag}}>
            {children}
        </TagStreamer.Provider>
    )
}

export function updateTag(tag_path, desired_value) {
    let data = {"tag_path": tag_path, "desired_value": desired_value}
    socket.emit("update_tag", data);
}