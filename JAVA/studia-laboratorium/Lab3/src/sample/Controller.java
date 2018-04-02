package sample;

import javafx.beans.property.SimpleStringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.ProgressBarTableCell;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;

import java.io.File;
import java.net.URL;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;
import java.util.ResourceBundle;

public class Controller implements Initializable {
    @FXML TableView<ImageProcessingJob> imagesTable;
    @FXML TableColumn<ImageProcessingJob, String> imageNameColumn;
    @FXML TableColumn<ImageProcessingJob, Double> progressColumn;
    @FXML TableColumn<ImageProcessingJob, String> statusColumn;
    @FXML Button directoryChoose


    private ObservableList<ImageProcessingJob> jobs = FXCollections.observableArrayList();
    private File outputDir = new File(".");

    @Override
    public void initialize(URL location, ResourceBundle resources) {
        imageNameColumn.setCellValueFactory(param -> new SimpleStringProperty(param.getValue().getFileName()));
        statusColumn.setCellValueFactory(param -> param.getValue().getStatusProperty());
        progressColumn.setCellFactory(ProgressBarTableCell.<ImageProcessingJob>forTableColumn());
        progressColumn.setCellValueFactory(param -> param.getValue().getProgessProperty().asObject());

        imagesTable.setItems(jobs);
    }

    public void AddFiles(ActionEvent actionEvent) {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setInitialDirectory(new File("images"));
        fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("JPG images", "*.jpg"));
        List<File> selectedFiles = fileChooser.showOpenMultipleDialog(null);
        selectedFiles.forEach(file -> jobs.add(new ImageProcessingJob(Paths.get(file.getAbsolutePath()))));
    }

    public void ChooseDirectory(ActionEvent actionEvent) {
        DirectoryChooser directoryChooser = new DirectoryChooser();
        directoryChooser.setInitialDirectory(new File("."));
        outputDir = directoryChooser.showDialog(null);
        directoryChoose.setText("/"+outputDir.getName());
    }

    public void ProcessImages(ActionEvent actionEvent) {
        new Thread(this::BackgroundJob).start();
    }

    private void BackgroundJob(){
        try {
            jobs.stream().forEach(imageProcessingJob -> imageProcessingJob.convertToGrayscale(outputDir));
        }catch(Exception e){
            e.printStackTrace();
        }
    }
}
