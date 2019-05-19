var express = require('express');
var router = express.Router();
const signin = require('./signin')
const signup = require('./signup')
router.use('/signup',signup)
router.use('/signin',signin)

module.exports = router;