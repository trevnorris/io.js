'use strict';
// Clear disk cache with:
//   sudo sh -c 'free && sync && echo 3 > /proc/sys/vm/drop_caches && free'

const fs = require('fs');
const Module = require('module');
const print = process._rawDebug;
const root_dir = process.cwd() + '/a_lot_of_files/';
print('writing files to', root_dir);

const BREADTH = 6;
const FILES = 10;

process.on('SIGINT', () => process.exit());

process.on('uncaughtException', (e) => {
  print(e.stack);
  process.exit(1);
});

process.on('exit', () => {
  print('cleaning up files');
  cleanupFilesnFolders(root_dir);
});


// Run the benchmark for cached and not cached.
let dir_array = genStructure();
let t = process.hrtime();
for (let i = 0; i < dir_array.length; i++) {
  require(dir_array[i]);
}
t = process.hrtime(t);
print('Cached:',
      ((t[0] * 1e6 + t[1] / 1e3) / dir_array.length).toFixed(2) + ' us/op');
print(''+dir_array.length + ' files required\n');
cleanupFilesnFolders(root_dir);


// Run the benchmark for cached and not cached.
dir_array = genStructure();
t = process.hrtime();
for (let i = 0; i < dir_array.length; i++) {
  require(dir_array[i]);
  Module._realpathCache = {};
}
t = process.hrtime(t);
print('Uncached:',
      ((t[0] * 1e6 + t[1] / 1e3) / dir_array.length).toFixed(2) + ' us/op');
print(''+dir_array.length + ' files required\n');
cleanupFilesnFolders(root_dir);


function mkdir(path) {
  // try catch or something?
  fs.mkdirSync(root_dir + path);
}


function mksym(srcpath, destpath) {
  fs.symlinkSync(root_dir + srcpath, root_dir + destpath);
}


function genFolderName(al, nu) {
  return al + '-' + nu + '-' + Math.random().toString(36).substr(2, 6);
}


function cleanupFilesnFolders(dir) {
  const ls = (() => { try { return fs.readdirSync(dir) } catch (e) { }})();
  if (!Array.isArray(ls)) return;
  for (let i = 0; i < ls.length; i++) {
    try {
      const stat = fs.lstatSync(dir + '/' + ls[i]);
      try {
        if (stat.isDirectory())
          cleanupFilesnFolders(dir + '/' + ls[i]);
        else
          fs.unlinkSync(dir + '/' + ls[i]);
      } catch (e) { }
    } catch (e) { }
  }
  fs.rmdirSync(dir);
}


function genStructure() {
  try { fs.mkdirSync(root_dir) } catch (e) { if (e.code != 'EEXIST') throw e }

  // First create a bunch of files and folders.
  const dir_array = [];  // Array of all paths, for benchmark below.
  // Make deep folder structore with files.
  for (let a = 0; a < BREADTH; a++) {
    const aname = genFolderName('a', a);
    const a_rpath = aname;  // "rpath" -> relative path
    mkdir(a_rpath);

    for (let b = 0; b < BREADTH; b++) {
      const bname = genFolderName('b', b);
      const b_rpath = a_rpath + '/' + bname;
      mksym(a_rpath, b_rpath);

      for (let c = 0; c < BREADTH; c++) {
        const cname = genFolderName('c', c);
        const c_rpath = b_rpath + '/' + cname;
        mkdir(c_rpath);

        for (let d = 0; d < BREADTH; d++) {
          const dname = genFolderName('d', d);
          const d_rpath = c_rpath + '/' + dname;
          mkdir(d_rpath);

          for (let e = 0; e < FILES; e++) {
            const ename = genFolderName('e', e);  // Actually a file
            const full_path = root_dir + d_rpath + '/' + ename + '.js';
            dir_array.push(full_path);
            fs.writeFileSync(full_path, 'module.exports = "' + full_path + '"');
          }
        }
      }
    }
  }

  return dir_array;
}
