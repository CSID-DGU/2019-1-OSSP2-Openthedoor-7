const express = require('express');
const router = express.Router();

const db = require('../module/pool.js')

router.get('/:score', async (req,res) => {

    let score = req.params.score

    console.log(score.toString());

    if(score<0){
        res.status(400).send({
            message : "Not Index Number"
        });
    } else {
        let checkQuery = "SELECT * FROM tetris_info.board WHERE score > ?";
        let checkResult = await db.queryParam_Arr(checkQuery,[score]);

        if(!checkResult){
            console.log("checkResult Error!");
            res.status(500).send({
                message : "Internal Server Error!"
            });
        } else {
            res.status(201).send({
                status : "true",
                message : "success",
                rank: checkResult.length,
            });
        }
    }
});

module.exports = router;