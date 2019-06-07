const express = require('express');
const router = express.Router();

const crypto = require('crypto-promise');		// crypto 모듈의 promise 버전
const db = require('../module/pool.js');

// POST 방식, ip:3000/signup
router.post('/', async (req, res) => {
	let user_id = req.body.user_id;
	let user_pw = req.body.user_pw;

	if (!user_id || !user_pw ) {
		res.status(400).send({
			message : "Null Value"
		});
	} else {
		// user_id를 가지고 DB를 검색해 그에 해당하는 모든 정보를 가져옴(salt, hashedpw)
		let checkQuery = 'SELECT * FROM openthedoor.user WHERE user_id = ?';
		let checkResult = await db.queryParam_Arr(checkQuery, [user_id]);	

		if (!checkResult) {												// 정상적으로 query문이 수행되지 않았을 경우
			res.status(500).send({
				message : "Internal Server Error"
			});
		} else if (checkResult.length === 1) {		// user_id 가 존재하는 경우
			let hashedpwd = await crypto.pbkdf2(user_pw, checkResult[0].user_salt, 100000, 32, 'sha512');	// 입력받은 pw를 DB에 존재하는 salt로 hashing
	    if (hashedpwd.toString('base64') === checkResult[0].user_pw) {	// hashedpw와 DB에 저장된 해싱된 pw 비교 => 같으면 제대로 된 비밀번호 입력
	      res.status(201).send({
	        message: "Login Success",
	        user_idx : checkResult[0].user_idx	// user_idx를 클라이언트에게 보내줌
	      });
	    } else {
	      res.status(400).send({
	        message : "Login Failed" ,
		user_idx : "-2"// 클라이언트에게는 login에러라고만 보내줌
	      });
	      console.log("pwd error");							// 콘솔창에는 어떤 에러인지 표시
	    }
		} else {
			res.status(400).send({
				message : "Login Failed",
				user_idx : "-1"// 클라이언트에게는 login에러라고만 보내줌
			});
			console.log("id error");								// 콘솔창에는 어떤 에러인지 표시
		}
	}
});

module.exports = router;
