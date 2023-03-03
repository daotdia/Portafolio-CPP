FROM gcc:latest
WORKDIR /app
COPY apuestas.cpp /app
RUN g++ -pthread ./apuestas.cpp -o apuestas
CMD ["./apuestas"]