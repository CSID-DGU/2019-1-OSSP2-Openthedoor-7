const express = require('express');
const router = express.Router();

const db = require('../module/pool.js')

router.get('/:user_idx', async (req,res) => {

    let user_idx = req.params.user_idx;

    console.log(typeof user_idx);

    if(user_idx<0){
        res.status(400).send({
            message : "Not Index Number"
        });
    } else {
        let checkQuery = "SELECT * FROM openthedoor.user WHERE user_idx= ?";
        let checkResult = await db.queryParam_Arr(checkQuery,[user_idx]);

        if(!checkResult){
            console.log("checkResult Error!");
            res.status(500).send({
                message : "Internal Server Error!"
            });
        } else if(checkResult.length === 0){
            res.status(400).send({
                status : "false",
                message : "No user"
            });
        
        } 
        checkQuery = "SELECT user_id,score, @vRank := @vRank +1 AS rank FROM openthedoor.user AS p, (SELECT @vRank :=0) AS R ORDER BY score DESC";
        let rankResult = await db.queryParam_Arr(checkQuery,[]);

        if(rankResult.length === 0){
            res.status(400).send({
                status : "false",
                message : "no users"
            });
        
        } 
        else{
            console.log(checkResult[0].user_id);
            console.log(rankResult);
            res.status(201).send({
                status : "true",
                message : "success",
                user_id : checkResult[0].user_id, 
                score: rankResult
            });
        }
    }
});

module.exports = router;