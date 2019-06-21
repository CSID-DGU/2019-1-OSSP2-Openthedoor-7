var express = require('express');
var router = express.Router();

router.use('/signup', require('./signup.js'));
router.use('/signin', require('./signin.js'));

router.use('/score',require('./score.js'));
router.use('/rank',require('./rank.js'));
router.use('/first',require('./first.js'));
router.use('/ranking',require('./ranking.js'));

module.exports = router;

/* GET home page. */
/*
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});
*/
