import serial
import time
import statistics
import firebase_admin
from firebase_admin import credentials, db

# Firebase configuration
cred = credentials.Certificate("E:/Python/smart-box-4da35-firebase-adminsdk-fbsvc-1af00c25a5.json")
firebase_admin.initialize_app(cred, {
    "databaseURL": "https://smart-box-4da35-default-rtdb.asia-southeast1.firebasedatabase.app"
})

# Serial configuration
serial_port = "COM7"
baud_rate = 115200

# Data arrays
num_values = 20
bpm_values = []
temp_values = []

# Open serial connection
try:
    ser = serial.Serial(serial_port, baud_rate, timeout=1)
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    exit(1)

def capture_data():
    global bpm_values, temp_values
    bpm_values = []
    temp_values = []
    
    print("Waiting for data collection to start...")
    collecting = False
    
    while True:
        line = ser.readline().decode("utf-8", errors='ignore').strip()
        if line:
            if line == "START_DATA":
                print("Starting to collect data...")
                collecting = True
                continue
            elif line == "END_DATA":
                print("Data collection complete!")
                break
            elif collecting and "BPM:" in line and "Temperature:" in line:
                try:
                    parts = line.split(", ")
                    bpm_part = parts[0].split(": ")[1]
                    temp_part = parts[1].split(": ")[1]
                    bpm = float(bpm_part)
                    temp = float(temp_part)
                    
                    bpm_values.append(bpm)
                    temp_values.append(temp)
                    print(f"Collected value {len(bpm_values)}/{num_values}")
                except (IndexError, ValueError) as e:
                    print(f"Error parsing data line: {e}")
            else:
                print(f"Status: {line}")
    
    return len(bpm_values) == num_values and len(temp_values) == num_values

def calculate_average(values):
    if not values:
        return 0
    return sum(values) / len(values)

def send_to_firebase(avg_bpm, avg_temp):
    ref = db.reference("patient")
    try:
        ref.update({
            "avgHeartRate": avg_bpm,
            "avgTemperature": avg_temp
        })
        print(f"Sent to Firebase - Average Heart rate: {avg_bpm} bpm, Average Temperature: {avg_temp:.2f} °C")
        return True
    except Exception as e:
        print(f"Error sending data to Firebase: {e}")
        return False

def main():
    try:
        while True:
            if capture_data():
                avg_bpm = calculate_average(bpm_values)
                avg_temp = calculate_average(temp_values)
                print(f"Average Heart rate: {avg_bpm} bpm, Average Temperature: {avg_temp:.2f} °C")
                
                if send_to_firebase(avg_bpm, avg_temp):
                    print("Data successfully processed and sent to Firebase")
                    print("\nWaiting for next session...")
                    
            else:
                print("Failed to collect complete dataset. Waiting for new session...")
            
            time.sleep(1)
                
    except KeyboardInterrupt:
        print("\nProgram terminated by user")
    finally:
        ser.close()
        print("Serial port closed")

if __name__ == "__main__":
    main()