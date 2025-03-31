#!/usr/bin/env python3

import os, stat, errno
import requests

try:
    import _find_fuse_parts
except ImportError:
    pass

import fuse
from fuse import Fuse

if not hasattr(fuse, '__version__'):
    raise RuntimeError("your fuse-py doesn't know of fuse.__version__, probably it's too old.")

fuse.fuse_python_api = (0, 2)

participation_path = '/participation'

class MyStat(fuse.Stat):
    def __init__(self):
        self.st_mode = 0
        self.st_ino = 0
        self.st_dev = 0
        self.st_nlink = 0
        self.st_uid = 1000
        self.st_gid = 1000
        self.st_size = 0
        self.st_atime = 0
        self.st_mtime = 0
        self.st_ctime = 0

class WebServiceFS(Fuse):

    def getattr(self, path):
        st = MyStat()
        if path == '/':
            st.st_mode = stat.S_IFDIR | 0o755
            st.st_nlink = 2
        elif path == participation_path:
            st.st_mode = stat.S_IFREG | 0o666
            st.st_nlink = 1
            content = self.myRead(path, 0, 0)
            st.st_size = len(content)
        else:
            return -errno.ENOENT
        return st

    def readdir(self, path, offset):
        for r in  '.', '..', participation_path[1:]:
            yield fuse.Direntry(r)

    def open(self, path, flags):
        if path != participation_path:
            return -errno.ENOENT
        accmode = os.O_RDONLY | os.O_WRONLY | os.O_RDWR
        if (flags & accmode) not in (os.O_RDONLY, os.O_WRONLY, os.O_RDWR):
            return -errno.EACCES

    def myRead(self, path, size, offset):
        if path != participation_path:
            return -errno.ENOENT
        try:
            req = requests.get('https://mis.cp.eng.chula.ac.th/krerk/teaching/2022s2-os/status.php')
            content = req.text.encode("utf-8")
            if offset < len(content):
                return content[offset:offset+size]
            else:
                return b""
        except Exception as e:
            print("myRead error:", e)
            return b""

    def read(self, path, size, offset):
        return self.myRead(path, size, offset)

    def myWrite(self, path, buf, offset):
        if path != participation_path:
            return -errno.ENOENT

        try:
            raw = buf.decode("utf-8").strip().split(':')
            if len(raw) != 3:
                return -errno.EINVAL

            checkInUrl = 'https://mis.cp.eng.chula.ac.th/krerk/teaching/2022s2-os/checkIn.php'
            params = {
                'studentid': raw[0],
                'name': raw[1],
                'email': raw[2]
            }
            requests.post(checkInUrl, data=params)
            return len(buf)
        except Exception as e:
            print("myWrite error:", e)
            return -errno.EIO

    def write(self, path, buf, offset):
        return self.myWrite(path, buf, offset)

def main():
    usage = """
Web Service Participation Filesystem
""" + Fuse.fusage
    server = WebServiceFS(
        version="%prog " + fuse.__version__,
        usage=usage,
        dash_s_do='setsingle'
    )
    server.parse(errex=1)
    server.main()

if __name__ == '__main__':
    main()
