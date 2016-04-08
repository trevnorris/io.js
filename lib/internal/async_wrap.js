'use strict';

const async_wrap = process.binding('async_wrap');
const uidArray = async_wrap.getUidArray();

module.exports.getUid =
    process.binding('os').isBigEndian ? getUidBE : getUidLE;


function getUidLE() {
  if (uidArray[0] === 0xffffffff) {
    uidArray[0] = 0;
    uidArray[1]++;
  }
  return uidArray[0] + uidArray[1] * 0x100000000;
}

function getUidBE() {
  if (uidArray[1] === 0xffffffff) {
    uidArray[1] = 0;
    uidArray[0]++;
  }
  return uidArray[1] + uidArray[0] * 0x100000000;
}
