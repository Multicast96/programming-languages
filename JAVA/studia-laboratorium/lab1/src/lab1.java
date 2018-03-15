class lab1 {
    public static void main(String[] args) {
        DiskDirectory root = null;
        if(args[0] == null) return;
        if(args.length == 2){
            if(args[1].equals("SortByDate")) root = new DiskDirectory(args[0], false);
            else if(args[1].equals("SortByName")) root = new DiskDirectory(args[0], true);

        }
        if(root != null)root.print(0);
    }
}