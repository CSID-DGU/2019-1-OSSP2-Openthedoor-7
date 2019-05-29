const express = require('express');
const router = express.Router();

const db = require('../module/pool.js')

router.get('/:user_idx', async (req,res) => {

    let user_idx = req.params.user_idx

    console.log(user_idx.toString());

    if(user_idx<0){
        res.status(400).send({
            message : "Not Index Number"
        });
    } else {
        let checkQuery = "SELECT user.score FROM openthedoor.user WHERE user_idx= ?";
        let checkResult = await db.queryParam_Arr(checkQuery,[user_idx]);

        if(!checkResult){
            console.log("checkResult Error!");
            res.status(500).send({
                message : "Internal Server Error!"
            });
        } else if(checkResult.length === 0){
            res.status(400).send({
                status : "false",
                message : "There is not score"
            });
        
        } else {
            res.status(201).send({
                status : "true",
                message : "success",
                score: checkResult
            });
        }
    }
});

router.put('/:user_idx', async (req,res) => {
    let score = req.body.score;
    let user_idx = req.params.user_idx

    console.log(user_idx.toString());

    if(user_idx<0){
        res.status(400).send({
            message : "Not Index Number"
        });
    } else {
        let updateQuery = "UPDATE openthedoor.user SET score = ? WHERE user_idx= ?";
        let updateResult = await db.queryParam_Arr(updateQuery, [score, user_idx]);

        if (!updateResult) {                                        // 정상적으로 query문이 수행되지 않았을 경우
                res.status(500).send({
                    message : "Internal Server Error"
                });
            } else {
                res.status(201).send({
                    message : "Successfully Update Item",
                    score : updateResult
                });
            }
    }
});

module.exports = router;