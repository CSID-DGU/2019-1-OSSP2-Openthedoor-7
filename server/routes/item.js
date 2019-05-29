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
        let checkQuery = "SELECT tetris_info.board.item FROM tetris_info.board WHERE user_idx= ?";
        let checkResult = await db.queryParam_Arr(checkQuery,[user_idx]);

        if(!checkResult){
            console.log("checkResult Error!");
            res.status(500).send({
                message : "Internal Server Error!"
            });
        } else if(checkResult.length === 0){
            res.status(400).send({
                status : "false",
                message : "There is not item"
            });
        
        } else {
            res.status(201).send({
                status : "true",
                message : "success",
                item: checkResult
            });
        }
    }
});

router.put('/:user_idx', async (req,res) => {
    let item = req.body.item;
    let user_idx = req.params.user_idx

    console.log(user_idx.toString());

    if(user_idx<0){
        res.status(400).send({
            message : "Not Index Number"
        });
    } else {
        let updateQuery = "UPDATE tetris_info.board SET item = ? WHERE user_idx= ?";
        let updateResult = await db.queryParam_Arr(updateQuery, [item, user_idx]);

        if (!updateResult) {                                        // 정상적으로 query문이 수행되지 않았을 경우
                res.status(500).send({
                    message : "Internal Server Error"
                });
            } else {
                res.status(201).send({
                    message : "Successfully Update Item",
                    item : updateResult
                });
            }
    }
});

module.exports = router;