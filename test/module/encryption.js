const crypto = require('crypto-promise')
const encryption ={

    asyncCipher : (pw)=>{
        return new Promise(async (resolve)=>{
        let _salt = (await crypto.randomBytes(32)).toString('base64')
        let _cryptoPw   = (await crypto.hmac('sha1',_salt)(pw)).toString('base64')
       resolve( {cryptoPw : _cryptoPw , salt : _salt  })
        })
    },
    asyncVerifyConsistency : (pw,salt,dbPw)=>{
       return new Promise(async(resolve,reject)=>{


           if((await crypto.hmac('sha1',salt)(pw)).toString('base64') === dbPw)
           {
            resolve()
           }
           else{
            reject()
           }
       })   
   }
}
module.exports = encryption