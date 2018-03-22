package com.kdakus;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {

    public static void main(String[] args) {
        int serverPort = 1337;
        ExecutorService executor = Executors.newFixedThreadPool(2);
        boolean useThreadPool = true;

        try (ServerSocket serverSocket = new ServerSocket(serverPort)) {
            System.out.println("Serwer plików rozpoczyna pracę, na porcie: "+serverSocket.toString());
            while (true) {
                System.out.println("Nasłuchiwanie połączeń od klientów.");
                final Socket socket = serverSocket.accept();
                System.out.println("Połączenie przychodzące - rozpoczynam transfer pliku");
                if(useThreadPool){
                    executor.submit(new Task(socket,"download"));
                }else{
                    Thread t = new Thread(new Task(socket, "download"));
                    t.start();
                }

            }
        } catch (IOException e) {
            System.out.println("Błąd tworzenia serwera plików.");
            System.out.println(e.getMessage());
        }
    }
}


class Task implements Runnable{
    private Socket connection;
    private Path outputDir;

    public Task(Socket connection, String downloadDir){
        outputDir = Paths.get(downloadDir);
        this.connection = connection;

    }

    @Override
    public void run() {
        try(BufferedInputStream in = new BufferedInputStream(connection.getInputStream())){

            byte[] buffer = new byte[4096];
            int readBytes;
            int totalRead = 0;
            String fileName = "tmp_download_file";



            if(in.read(buffer,0,4) != -1){
                int name_length = ByteBuffer.wrap(buffer).getInt(); //długość nazwy

                if(in.read(buffer,0,name_length) != -1){
                    fileName = new String(buffer, 0, name_length);
                }
            }


            try(BufferedOutputStream out = new BufferedOutputStream(Files.newOutputStream(outputDir.resolve(fileName)))){
                while ((readBytes = in.read(buffer)) != -1) {
                    out.write(buffer, 0, readBytes);
                    totalRead += readBytes;
                }
            }
            System.out.println(fileName +" odebrano "+totalRead+" B.");

        }catch (Exception e){
            e.printStackTrace();
        }
    }
}
