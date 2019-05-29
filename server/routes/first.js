const express = require('express');
const router = express.Router();

const db = require('../module/pool.js')

router.get('/', async (req,res) => {

    let checkQuery = "SELECT user_ID FROM openthedoor.user";
    let checkResult = await db.queryParam_None(checkQuery);
    console.log('here');
    if(!checkResult){
        console.log("checkResult Error!");

        res.status(500).send({
            message : "Internal Server Error!"
        });
    } else {
        res.status(201).send({
            status : "true",
            message : "success",
            first: checkResult,
        });
    }
    
});

module.exports = router;
