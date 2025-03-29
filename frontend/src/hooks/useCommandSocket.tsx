import { useEffect, useState, useRef, useCallback } from "react";

const COMMAND_SERVER = "ws://192.168.7.2:9001";

/**
 * Commands:
 * 
 * - "count": Receives people count
 * - "left": Moves pan left
 * - "right": Moves pan right
 * - "up": Moves tilt up
 * - "down": Moves tilt down
 * - "alarm": Triggers alarm
 * - "stop": Terminates program
 */
export type CommandType = "count" | "left" | "right" | "up" | "down" | "alarm" | "stop";

export function useCommandSocket() {
  const [peopleCount, setPeopleCount] = useState<number>(0);
  const websocketRef = useRef<WebSocket>(undefined);

  // Send a command to the server
  const sendCommand = useCallback((command: CommandType) => {
    if (websocketRef.current && websocketRef.current.readyState === WebSocket.OPEN) {
      websocketRef.current.send(command);
    } else {
      console.error("WebSocket is not open. Can't send command: ", command);
    }
  }, []);

  // Establish socket connection and listen for messages
  useEffect(() => {
    const websocket = new WebSocket(COMMAND_SERVER);
    websocketRef.current = websocket;

    websocket.onopen = () => console.log("Command WebSocket connected");

    websocket.onclose = () => console.warn("Command WebSocket closed");

    websocket.onerror = (err) => console.error("Command WebSocket error:", err);

    websocket.onmessage = (event) => {
      try {
        const messageData = JSON.parse(event.data);

        // TODO: Identify commands and message

        console.log(messageData);
      
        setPeopleCount(peopleCount + 1);
      } catch (error) {
        console.error("Invalid JSON received:", event.data);
      }
    };

    return () => {
      websocket.close();
    };
  }, []);

  return { peopleCount, sendCommand };
}
