package sample;

import javafx.application.Platform;
import javafx.beans.property.DoubleProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.beans.property.SimpleStringProperty;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;

public class ImageProcessingJob {
    enum Status{
        WAITING,
        PROCESSING,
        DONE,
        ERROR
    }
    private SimpleStringProperty status;
    private DoubleProperty progess;
    private Path file;

    public ImageProcessingJob(Path file){
        this.file = file;
        status = new SimpleStringProperty(Status.WAITING.toString());
        progess = new SimpleDoubleProperty(0);
    }

    public SimpleStringProperty getStatusProperty(){
        return status;
    }

    public void setStatusProperty(Status s){
        status.set(s.toString());

        if(s == Status.WAITING){
            progess.setValue(0);
        }
    }

    public DoubleProperty getProgessProperty() {
        return progess;
    }

    public String getFileName(){
        return file.getFileName().toString();
    }

    public void convertToGrayscale(
            File outputDir //katalog docelowy
    ) {
        try {
            status.set(Status.PROCESSING.toString());
            //wczytanie oryginalnego pliku do pamięci
            BufferedImage original = ImageIO.read(file.toFile());

            //przygotowanie bufora na grafikę w skali szarości
            BufferedImage grayscale = new BufferedImage(
                    original.getWidth(), original.getHeight(), original.getType());
            //przetwarzanie piksel po pikselu
            for (int i = 0; i < original.getWidth(); i++) {
                for (int j = 0; j < original.getHeight(); j++) {
                    //pobranie składowych RGB
                    int red = new Color(original.getRGB(i, j)).getRed();
                    int green = new Color(original.getRGB(i, j)).getGreen();
                    int blue = new Color(original.getRGB(i, j)).getBlue();
                    //obliczenie jasności piksela dla obrazu w skali szarości
                    int luminosity = (int) (0.21*red + 0.71*green + 0.07*blue);
                    //przygotowanie wartości koloru w oparciu o obliczoną jaskość
                    int newPixel = new Color(luminosity, luminosity, luminosity).getRGB();
                    //zapisanie nowego piksela w buforze
                    grayscale.setRGB(i, j, newPixel);
                }
                //obliczenie postępu przetwarzania jako liczby z przedziału [0, 1]
                double progress = (1.0 + i) / original.getWidth();
                //aktualizacja własności zbindowanej z paskiem postępu w tabeli
                Platform.runLater(() -> progess.set(progress));
            }
            //przygotowanie ścieżki wskazującej na plik wynikowy
            Path outputPath = Paths.get(outputDir.getAbsolutePath(), file.getFileName().toString());

            //zapisanie zawartości bufora do pliku na dysku
            ImageIO.write(grayscale, "jpg", outputPath.toFile());
            setStatusProperty(Status.DONE);
        } catch (IOException ex) {
            //translacja wyjątku
            setStatusProperty(Status.ERROR);
            throw new RuntimeException(ex);
        }
    }

}
