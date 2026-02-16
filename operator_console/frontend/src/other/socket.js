import ReconnectingWebSocket from 'reconnecting-websocket';

class CrowSocket {
    constructor(url) {
        //websocket connection address
        this.url = url;

        //make the socket connection
        this.reconnection_socket = new ReconnectingWebSocket(url);

        //establish message haneler function
        this.reconnection_socket.onmessage = (event) => {
            this.process_message(event.data);
        };

        //event dict
        this.event_dict = {};
    }

    //sends message in CrowSocket event format
    emit(event_name, data) {
        //send message as JSON
        this.reconnection_socket.send(JSON.stringify({"event_name": event_name, "data": data}));
    }

    //adds function with event to dictionary
    on(event_name, callback_function) {
        //add to event dict
        this.event_dict[event_name] = callback_function;
    }

    //removes function with event from message list
    off(event_name) {
        //remove from event dict
        for (let key in this.event_dict) {
            if (key === event_name) {
                delete this.event_dict[key];
                break;
            }
        }
    }

    //processes message & finds / runs function that matches event
    process_message(event_data) {
        let recieved_message = JSON.parse(event_data);
        let event_name = recieved_message.event_name;
        let data = recieved_message.data;

        //find function handle that matches event name
        for (let key in this.event_dict) {
            if (key === event_name) {
                //run function and pass in data
                this.event_dict[key](data);
            }
        }
    }

    reconnection_error_function() {
    }

    //on open
    onConnection(callback_function) {
        this.reconnection_socket.onopen = callback_function;
    }

    //on disconnect
    onDisconnect(callback_function) {
        this.reconnection_socket.onclose = callback_function;
    }

    //on error
    onError(callback_function) {
        this.reconnection_socket.onerror = callback_function;
    }
}

//create socket
//const url = "ws://127.0.0.1:5000/ws";

const host = window.location.hostname; // Gets the IP or domain
//const port = window.location.port; // Gets the port number
const port = 5000; // port hard set 5000 for now.
const url = "ws://"+host+":"+port+"/ws"
export const socket = new CrowSocket(url);


