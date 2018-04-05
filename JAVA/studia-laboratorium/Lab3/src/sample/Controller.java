package sample;

import javafx.beans.property.SimpleStringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.*;
import javafx.scene.control.cell.ProgressBarTableCell;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;

import java.io.File;
import java.net.URL;
import java.nio.file.Paths;
import java.util.List;
import java.util.ResourceBundle;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.TimeUnit;

public class Controller implements Initializable {
    @FXML TableView<ImageProcessingJob> imagesTable;
    @FXML TableColumn<ImageProcessingJob, String> imageNameColumn;
    @FXML TableColumn<ImageProcessingJob, Double> progressColumn;
    @FXML TableColumn<ImageProcessingJob, String> statusColumn;
    @FXML Button directoryChoose;
    @FXML ComboBox<String> comboBox;
    @FXML TextField newPool;
    private long[] execTime = new long[3];

    enum Processing{
        SEQUENTIAL,
        CONCURENT_COMMON_POOL,
        CONCURENT_USER_POOL
    };

    Processing processingOption = Processing.SEQUENTIAL;


    private ObservableList<ImageProcessingJob> jobs = FXCollections.observableArrayList();
    private File outputDir = new File("processed_images");

    @Override
    public void initialize(URL location, ResourceBundle resources) {
        imageNameColumn.setCellValueFactory(param -> new SimpleStringProperty(param.getValue().getFileName()));
        statusColumn.setCellValueFactory(param -> param.getValue().getStatusProperty());
        progressColumn.setCellFactory(ProgressBarTableCell.<ImageProcessingJob>forTableColumn());
        progressColumn.setCellValueFactory(param -> param.getValue().getProgessProperty().asObject());
        comboBox.getItems().addAll("Sekwencyjne" , "Współbieżne common pool" , "Współbieżne użytkownika");
        newPool.setPromptText("Liczba wątków");
        comboBox.setOnAction(event -> {
            int index = comboBox.getSelectionModel().getSelectedIndex();
            switch (index){
                case 0:
                    processingOption = Processing.SEQUENTIAL;
                    newPool.setVisible(false);
                    break;
                case 1:
                    processingOption = Processing.CONCURENT_COMMON_POOL;
                    newPool.setVisible(false);
                    break;
                case 2:
                    processingOption = Processing.CONCURENT_USER_POOL;
                    newPool.setVisible(true);
                    break;
            }
        });
        comboBox.getSelectionModel().selectFirst();
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
        jobs.stream().forEach(job -> job.setStatusProperty(ImageProcessingJob.Status.WAITING));
        switch (processingOption){
            case SEQUENTIAL:
                new Thread(this::Sequential).start();
                break;
            case CONCURENT_COMMON_POOL:
                new Thread(() -> Concurent(null)).start();
                break;
            case CONCURENT_USER_POOL:
                new Thread(() -> Concurent(new ForkJoinPool(Integer.valueOf(newPool.getText())))).start();
                break;
        }
    }

    private void Sequential(){
        long start = System.currentTimeMillis(); //zwraca aktualny czas [ms]
        jobs.stream().forEach(imageProcessingJob -> imageProcessingJob.convertToGrayscale(outputDir));
        execTime[0] = System.currentTimeMillis()-start; //czas przetwarzania [ms]
        show();
    }

    private void Concurent(ForkJoinPool pool){
        if(pool != null){
            long start = System.currentTimeMillis(); //zwraca aktualny czas [ms]
            pool.submit(()-> jobs.parallelStream().forEach(imgJob -> imgJob.convertToGrayscale(outputDir)));
            try {
                pool.shutdown();
                pool.awaitTermination(1 , TimeUnit.MINUTES);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            execTime[2] = System.currentTimeMillis()-start; //czas przetwarzania [ms]
            show();

        }else {
            long start = System.currentTimeMillis(); //zwraca aktualny czas [ms]
            jobs.parallelStream().forEach(imageProcessingJob -> imageProcessingJob.convertToGrayscale(outputDir));
            execTime[1] = System.currentTimeMillis()-start; //czas przetwarzania [ms]
            show();
        }
    }


    private void show(){
        StringBuilder sb = new StringBuilder();
        sb.append("Sekwencyjnie: ");
        sb.append(execTime[0]/1000.0);
        sb.append(" s\n");
        sb.append("Współbieżnie - common pool: ");
        sb.append(execTime[1]/1000.0);
        sb.append(" s\n");
        sb.append("Współbieżnie - podana liczba wątków: ");
        sb.append(execTime[2]/1000.0);
        sb.append(" s\n");
        System.out.print(sb.toString());
    }
}
