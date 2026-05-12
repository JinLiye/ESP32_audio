import argparse
import serial
import struct
import wave


def read_exact(ser, n):
    buf = bytearray()
    while len(buf) < n:
        chunk = ser.read(n - len(buf))
        if not chunk:
            raise TimeoutError(f"Timeout while reading {n} bytes, got {len(buf)}")
        buf.extend(chunk)
    return bytes(buf)


def main():
    p = argparse.ArgumentParser(description="Capture 5s PCM from ESP32 and save as WAV")
    p.add_argument("--port", required=True, help="Serial port, e.g. COM3")
    p.add_argument("--baud", type=int, default=115200)
    p.add_argument("--out", default="recording.wav")
    args = p.parse_args()

    with serial.Serial(args.port, args.baud, timeout=5) as ser:
        ser.reset_input_buffer()
        ser.write(b"R")

        magic = read_exact(ser, 4)
        if magic != b"AUD0":
            raise RuntimeError(f"Bad header magic: {magic!r}")

        sample_rate = struct.unpack("<I", read_exact(ser, 4))[0]
        num_samples = struct.unpack("<I", read_exact(ser, 4))[0]
        payload_size = num_samples * 2
        pcm = read_exact(ser, payload_size)

    with wave.open(args.out, "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)
        wf.setframerate(sample_rate)
        wf.writeframes(pcm)

    print(f"Saved {args.out}")
    print(f"Sample rate: {sample_rate} Hz, samples: {num_samples}, duration: {num_samples / sample_rate:.2f} s")


if __name__ == "__main__":
    main()
