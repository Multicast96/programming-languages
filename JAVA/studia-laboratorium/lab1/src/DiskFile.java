import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

public class DiskFile extends DiskElement {
    public DiskFile(File file){
        diskElement = file;
        lastModified = new Date(diskElement.lastModified());
    }

    @Override
    protected void print(int depth) {
        depth++;
        StringBuilder line = new StringBuilder();

        for(int i = 0; i < depth;i++) line.append("-");

        line.append(diskElement.getName());

        if(line.length() > DiskElement.maxNameLength){
            line.setLength(DiskElement.maxNameLength-3);
            line.append("...");
        }else{
            for(int i = line.length(); i < DiskElement.maxNameLength;i++) line.append(" ");
        }
        line.append(diskElement.isDirectory() ? " K " : " P ");
        line.append(new SimpleDateFormat("yyyy-MM-dd").format(lastModified));
        System.out.println(line);
    }
}
