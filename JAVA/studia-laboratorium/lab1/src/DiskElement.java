import java.io.File;
import java.util.Date;

public abstract class DiskElement implements Comparable<DiskElement> {
    public static final int maxNameLength = 20;
    protected File diskElement;
    protected Date lastModified;

    protected abstract void print(int depth);

    public Date getLastModified(){
        return lastModified;
    }

    @Override
    public String toString() {
        return diskElement.getName();
    }

    @Override
    public int hashCode() {
        return (int) (123 * (diskElement.getTotalSpace() + diskElement.hashCode()));
    }

    @Override
    public boolean equals(Object obj) {
        return obj.getClass() == this.getClass() && diskElement.getName().equals(obj.toString());
    }

    @Override
    public int compareTo(DiskElement o) {
        return diskElement.getName().compareTo(o.toString());
    }
}
