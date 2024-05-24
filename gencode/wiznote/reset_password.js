'use strict';

const program = require('commander');

program
  .option('-p, --password [value]', 'generate user password')
  .option('-s, --salt [value]', 'generate user salt')
  .parse(process.argv);

const salt = program.salt;
const password = program.password;
//console.log(password);

const crypto = require('crypto');

function md5(data) {
  let hash = crypto.createHash('md5').update(data).digest('hex');
  return hash;
}

function generateUserPassword() {
   const dbPassword =  md5(md5(md5(md5(password)))+ salt);
   console.log(dbPassword);
}

generateUserPassword();
