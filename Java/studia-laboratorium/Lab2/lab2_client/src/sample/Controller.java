package sample;

import javafx.beans.property.DoubleProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.*;
import javafx.scene.control.cell.ProgressBarTableCell;
import javafx.scene.text.Text;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;
import javafx.stage.Window;


import java.io.File;
import java.net.URL;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ResourceBundle;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import static java.lang.Math.random;

public class Controller implements Initializable{
    @FXML private Text fileName;
    @FXML private Text fileStatus;
    @FXML private ProgressBar progressBar;
    private ExecutorService executor = Executors.newSingleThreadExecutor();
    Task sendFileTask;

    @Override
    public void initialize(URL location, ResourceBundle resources) {
        sendFileTask = null;
    }

    public void clearCanvas(ActionEvent event) {
    }

    public void chooseFile(ActionEvent actionEvent) {
        if(sendFileTask != null && sendFileTask.getState().equals(Task.State.TRANSFER)){
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle("Błąd");
            alert.setHeaderText(null);
            alert.setContentText("Plik w trakcie wysyłania");
            alert.showAndWait();
            return;
        }
        Window window = fileName.getScene().getWindow();

        FileChooser directoryChooser = new FileChooser();
        File file = directoryChooser.showOpenDialog(window);

        if(file == null) return;

        sendFileTask = new Task(Paths.get(file.getAbsolutePath()));
        fileName.textProperty().bind(sendFileTask.getNameProperty());
        fileStatus.textProperty().bind(sendFileTask.getStateProperty());
        progressBar.progressProperty().bind(sendFileTask.getProgressProperty());
    }

    public void sendFile(ActionEvent event){
        if(sendFileTask == null || sendFileTask.getState().equals(Task.State.TRANSFER)){
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle("Błąd");
            alert.setHeaderText(null);

            if(sendFileTask == null) alert.setContentText("Najpierw wybierz plik do wysłania.");
            else if(sendFileTask.getState().equals(Task.State.DONE)) alert.setContentText("Ten plik został już wysłany.");
            else if(sendFileTask.getState().equals(Task.State.TRANSFER)) alert.setContentText("Ten plik jest w trakcie wysyłki");

            alert.showAndWait();
        }else{
            executor.submit(sendFileTask);
        }
    }
}
