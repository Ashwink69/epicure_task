import paho.mqtt.client as mqtt

BROKER = "172.20.10.3"      # Mosquitto is running on your laptop
PORT = 1883
TOPIC = "epicure/commands"

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker")
    else:
        print("Failed to connect, return code:", rc)

client = mqtt.Client()
client.on_connect = on_connect

client.connect(BROKER, PORT, keepalive=60)
client.loop_start()

print("MQTT Publisher Ready!")
print("Enter commands like: motor:100:1 or led:on")
print("--------------------------------------------")

try:
    while True:
        cmd = input("Command: ")
        if cmd.strip() != "":
            client.publish(TOPIC, cmd)
            print("Published:", cmd)

except KeyboardInterrupt:
    print("\nExiting...")
    client.loop_stop()
    client.disconnect()
