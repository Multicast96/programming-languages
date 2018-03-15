import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TreeSet;

public class DiskDirectory extends DiskElement {
    private TreeSet<DiskElement> childrens;

    public DiskDirectory(File directory, boolean first){
        if (first) childrens = new TreeSet<>();
        else    childrens = new TreeSet<>((o1, o2) -> {return o1.getLastModified().compareTo(o2.getLastModified());});
        diskElement = directory;
        lastModified = new Date(diskElement.lastModified());

        File[] childrens = diskElement.listFiles();
        if(childrens != null){
            for(File diskElement : childrens){
                if(diskElement.isDirectory()){
                    this.childrens.add(new DiskDirectory(diskElement, first));
                }else if(diskElement.isFile()){
                    this.childrens.add(new DiskFile(diskElement));
                }
            }
        }
    }

    public DiskDirectory(String path, boolean first){
        this(new File(path), first);
    }

    @Override
    protected void print(int depth) {
        depth++;
        String line = "";

        for(int i = 0; i < depth;i++) line += "-";

        line += diskElement.getName();

        if(line.length() > DiskElement.maxNameLength){
            line = line.substring(0,DiskElement.maxNameLength-3);
            line += "...";
        }else{
            for(int i = line.length(); i < DiskElement.maxNameLength;i++) line += " ";
        }
        line += diskElement.isDirectory() ? " K " : " P ";
        line += new SimpleDateFormat("yyyy-MM-dd").format(lastModified);
        System.out.println(line);

        for (DiskElement diskElement : this.childrens){
            diskElement.print(depth);
        }
    }
}
