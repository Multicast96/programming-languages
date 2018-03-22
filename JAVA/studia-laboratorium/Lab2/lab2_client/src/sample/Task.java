package sample;

import javafx.beans.property.DoubleProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.scene.control.Alert;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;

public class Task implements Runnable{
    public enum State{
        LOADED,
        TRANSFER,
        DONE,
        SERVER_ERROR
    }
    private Map<State, String> states = new HashMap<>();

    private State state;
    private SimpleDoubleProperty progressProperty;
    private SimpleStringProperty stateProperty;
    private SimpleStringProperty nameProperty;
    private Path file;

    public Task(Path file){
        states.put(State.LOADED, "Gotowy do wysłania");
        states.put(State.TRANSFER, "Wysyłanie");
        states.put(State.DONE, "Przesłano");
        states.put(State.SERVER_ERROR, "Błąd połączenia z serwerem.");

        progressProperty = new SimpleDoubleProperty(0);
        stateProperty = new SimpleStringProperty();
        nameProperty = new SimpleStringProperty(file.getFileName().toString());
        this.file = file;
        UpdateState(State.LOADED);
    }

    public SimpleDoubleProperty getProgressProperty(){
        return progressProperty;
    }

    public SimpleStringProperty getStateProperty(){
        return stateProperty;
    }

    public SimpleStringProperty getNameProperty(){
        return nameProperty;
    }

    public State getState(){
        return state;
    }

    public void UpdateState(State state){
        this.state = state;
        this.stateProperty.set(states.get(state));
    }

    public void UpdateProgress(double progress){
        this.progressProperty.set(progress);
    }

    @Override
    public void run() {
        UpdateState(State.TRANSFER);
        try(Socket socket = new Socket("localhost" , 1337)) {

            try(BufferedOutputStream out = new BufferedOutputStream(socket.getOutputStream());
            BufferedInputStream in = new BufferedInputStream(Files.newInputStream(file));){
                byte[] buffer = new byte[4096];
                int readBytes;
                int totalRead = 0;

                //name size
                out.write(ByteBuffer.allocate(4).putInt(file.getFileName().toString().getBytes("UTF-8").length).array()); // jedna liczba 4 bajtowa
                //name
                out.write(file.getFileName().toString().getBytes("UTF-8"));

                //file size
                //file

                while ((readBytes = in.read(buffer)) != -1){
                    out.write(buffer, 0, readBytes);
                    totalRead += readBytes;
                    UpdateProgress((double)totalRead/file.toFile().length());
                }
            }catch (Exception e){
                e.printStackTrace();
                UpdateState(State.SERVER_ERROR);
            }

            UpdateProgress(1);
            UpdateState(State.DONE);
        } catch (IOException e) {
            UpdateState(State.SERVER_ERROR);
            e.printStackTrace();
        }

    }
}
