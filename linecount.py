import os
import sys

BASEPATH = os.path.dirname(sys.argv[0])


def files(folder, *filetypes):
    f = []
    for e in os.listdir(folder):
        if os.path.isfile(folder+"/"+e):
            if len(filetypes)==0 or e.split(".")[-1] in filetypes:
                f.append(folder+"/"+e)
        else:
            f += files(folder+"/"+e, *filetypes)
    return f
    
    
def linecountFile(path):
    with open(path, "r") as f:
        count = len(f.readlines())
    return count
    
    
def linecount(path, *filetypes):
    return sum([linecountFile(f) for f in files(path, *filetypes)])
    
    
if __name__ == "__main__":
    print(linecount(sys.argv[1] if len(sys.argv)>1 else BASEPATH, *sys.argv[2:] if len(sys.argv)>2 else []))