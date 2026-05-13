import argparse
import socket


def main():
    parser = argparse.ArgumentParser(description="Simple UDP listener for ESP32 messages")
    parser.add_argument("--host", default="0.0.0.0", help="Host to bind, default: 0.0.0.0")
    parser.add_argument("--port", type=int, default=9000, help="UDP port to listen on, default: 9000")
    args = parser.parse_args()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((args.host, args.port))

    print(f"Listening on {args.host}:{args.port}")

    while True:
        data, addr = sock.recvfrom(2048)
        text = data.decode("utf-8", errors="replace")
        print(f"From {addr[0]}:{addr[1]} -> {text}")


if __name__ == "__main__":
    main()
