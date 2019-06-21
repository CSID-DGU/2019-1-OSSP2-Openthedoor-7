const express = require('express');
const router = express.Router();

const crypto = require('crypto-promise');		// crypto 모듈의 promise 버전
const db = require('../module/pool.js');

// POST 방식, ip:3000/signup
router.post('/', async (req, res) => {

	let user_id = req.body.user_id;
	let user_pw = req.body.user_pw;

	if (!user_id || !user_pw) {
		res.status(400).send({
			message : "Null Value"
		});
	} else {
		let checkQuery = 'SELECT * FROM openthedoor.user WHERE user_id = ?';		// 입력받은 user_id DB에 존재하는지 확인
		let checkResult = await db.queryParam_Arr(checkQuery, [user_id]);

		if (!checkResult) {												// 정상적으로 query문이 수행되지 않았을 경우
			res.status(500).send({
				message : "Internal Server Error"
			});
		} else if (checkResult.length === 1) {		// 배열의 길이 === 1 => DB에 user_id가 존재
			res.status(400).send({
				message : "Already Exists"
			});
		} else {																	
			const salt = await crypto.randomBytes(32);			
	    	const hashedpwd = await crypto.pbkdf2(user_pw, salt.toString('base64'), 100000, 32, 'sha512');	
	    	console.log('hashing success');	
	    	// password 해싱
			let insertUserQuery = 'INSERT INTO openthedoor.user (user_id, user_pw, user_salt) VALUES (?, ?, ?)';
			let insertUserResult = await db.queryParam_Arr(insertUserQuery, [user_id, hashedpwd.toString('base64'), salt.toString('base64')]);
			console.log('result의 객체 : ' + insertUserResult.insertId);
			
			if (!insertUserResult ) {										// 정상적으로 query문이 수행되지 않았을 경우
				res.status(500).send({
					message : "Internal Server Error"
				});
			} else {
				res.status(201).send({
					message : "Successfully Sign Up",
					user_idx : insertUserResult.insertId,
				});
			}
		}
	}
});

module.exports = router;
