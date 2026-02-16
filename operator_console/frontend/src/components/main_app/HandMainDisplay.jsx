import {Button} from "@/components/ui/button.jsx";
import {TagStreamer, updateTag} from "@/components/helper_components/BackendIntegration.jsx";
import {useContext, useEffect} from "react";
import {socket} from "@/other/socket.js";
import {Label} from "@/components/ui/label.jsx";


export function HandMainDisplay() {

    const {subscribeTag, unsubscribeTag, tagData} = useContext(TagStreamer)

    //on mount - subscribe to needed tags
    useEffect(() => {
        subscribeTag("HAOS:hand/led.actual_state.value")
        subscribeTag("HAOS:hand/A0_voltage.value")

        return ()=> {
            unsubscribeTag("HAOS:hand/led.actual_state.value")
            unsubscribeTag("HAOS:hand/A0_voltage.value")
        }
    }, []);

    return (
        <div className="p-4 flex flex-col gap-4 w-[300px]">
            <Button variant="outline"
                    onClick={()=>{
                        const current_value = tagData["HAOS:hand/led.actual_state.value"]
                        let value_to_update;

                        console.log(current_value)

                        if (current_value === "1") {
                            value_to_update = 0
                        } else {
                            value_to_update = 1
                        }
                        //update desired state
                        updateTag("HAOS:hand/led.desired_state.value", value_to_update)
                    }}
            >
                {tagData["HAOS:hand/led.actual_state.value"] === "1" ?
                    <Label>
                        LED ON
                    </Label>
                    :
                    <Label>
                        LED OFF
                    </Label>
                }
            </Button>

            <Button variant="outline" className="pointer-events-none">
                VOLTAGE: {tagData["HAOS:hand/A0_voltage.value"]}
            </Button>
        </div>
    )
}