var express = require('express');
var router = express.Router();
const path = require('path')
const modulePath  = path.join(__dirname,'../../../module')
const connection = require(path.join(__dirname,'../../../config/dbConfig.js'))
const encryption = require(path.join(modulePath,'./encryption.js'))
const responseMessage = require(path.join(modulePath,'./responseMessage.js'))
const utils = require(path.join(modulePath,'./utils.js'))
const statusCode = require(path.join(modulePath,'./statusCode.js'))
const selectUserQuery = "SELECT * FROM user WHERE id = ? "
router.post('/',async(req,res)=>{
let id = req.body.id
let password  =  req.body.password
if(!id ||!password)
{
//요청 바디값 오류
res.send(utils.successFalse(statusCode.BAD_REQUEST,responseMessage.NULL_VALUE))
return
}
connection.query(selectUserQuery,[id],async(err, result)=>{
  if(err)
  {
    res.send(utils.successFalse(statusCode.DB_ERROR,responseMessage.DB_ERR))

  }
  else if(result.length < 1)
  {
    res.send(utils.successFalse(statusCode.NO_CONTENT, responseMessage.ID_OR_PW_WRONG_VALUE))
  }
  else
  {
    await encryption.asyncVerifyConsistency(password,result[0].salt,result[0].password).then(()=>{

        let jsonResult ={
            userIdx : result[0].userIdx
        }
        res.send(utils.successTrue(statusCode.OK,responseMessage.LOGIN_SUCCESS,jsonResult))
    }).catch(()=>{
        res.send(utils.successFalse(statusCode.NO_CONTENT,responseMessage.ID_OR_PW_WRONG_VALUE))
    })
  }
 




})
    





})

module.exports = router;