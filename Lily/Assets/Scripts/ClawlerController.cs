using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//　敵キャラクターの動きを制御します
public class ClawlerController : MonoBehaviour {
    #region アニメーション用

    // 再生するアニメーションを指定します
    [SerializeField]
    private Object[] AnimationList;

    // アニメーションの種類を表します
    [SerializeField]
    enum AnimationPattern {
        Attack = 0,
        Walk = 1
    }

    // 敵キャラクターのモーションの状態を表します
    enum MotionState {
        Start,
        Walk,
        Attack
    }
    [SerializeField]
    MotionState motionState = MotionState.Start;

    // キャラクター位置座標のオフセットを指定します。
    [SerializeField]
    private Vector3 positionOffset = Vector3.zero;
    // キャラクターのスケールを指定します。
    [SerializeField]
    private Vector3 characterScale = Vector3.one;

    // SpriteStudio Anime を操作するためのクラス
    Script_SpriteStudio6_Root scriptRoot = null;

    #endregion

    // 敵キャラクターの移動速度を指定します
    [SerializeField]
    private float walkSpeed = 1.0f;

    // 敵キャラクターの行動範囲の端を指定します
    private Transform[] moveLimits;

    // プレイヤー判定用の対象レイヤーを指定します
    [SerializeField]
    private LayerMask playerLayer;
    // 壁判定用の対象レイヤーを指定します
    [SerializeField]
    private LayerMask groundLayer;

    // 壁判定用のコライダーを指定します
    [SerializeField]
    private BoxCollider2D wallChecker = null;

    // コンポーネント参照用の変数
    new Rigidbody2D rigidbody2D;
    // Start is called before the first frame update
    void Start() {
        rigidbody2D = GetComponent<Rigidbody2D>();
        AnimationStart();
    }

    // Update is called once per frame
    void Update() {
        switch (motionState) {
            case MotionState.Start:
                UpdateForStartState();
                break;
            case MotionState.Walk:
                UpdateForWalkState();
                break;
            case MotionState.Attack:
                UpdateForAttackState();
                break;
            default:
                break;
        }
    }

    private void SetWalkState() {
        motionState = MotionState.Walk;
        ChangeAnimation(AnimationPattern.Walk);
    }

    private void SetAttackState() {

        motionState = MotionState.Attack;
        ChangeAnimation(AnimationPattern.Attack);
    }

    private void UpdateForStartState() {
        SetWalkState();
    }

    private void UpdateForWalkState() {
        // 等速度運動
        var velocity = rigidbody2D.velocity;
        velocity.x = walkSpeed;
        rigidbody2D.velocity = velocity;
    }

    private void UpdateForAttackState() {

    }

    // プレイヤーと接触したら攻撃モーションへ遷移します
    private void OnCollisionEnter2D(Collision2D collision) {
        var layerId = 1 << collision.gameObject.layer;

        if ((layerId & playerLayer) != 0) {
            SetAttackState();
        }

        // 地面との交差判定
        Vector2 point = wallChecker.transform.position;
        point += wallChecker.offset;
        var result = Physics2D.OverlapBox(
            point,
            wallChecker.size,
            wallChecker.transform.rotation.eulerAngles.z,
            groundLayer);

        if (result) {
            walkSpeed *= -1;
            if (transform.localScale.x > 0) {
                transform.localScale = new Vector3(1, 1, 1);
            }
            else if (transform.localScale.x < 0) {
                transform.localScale = new Vector3(-1, 1, 1);
            }
        }
    }

    #region アニメーション用関数
    // アニメーション開始 
    private void AnimationStart() {
        int listLength = AnimationList.Length;

        // すでにアニメーション生成済 or リソース設定無い場合はreturn
        if (scriptRoot != null || listLength < 1)
            return;

        // 再生するリソース名をリストから取得して再生する
        Object resourceObject = AnimationList[0];
        if (resourceObject != null) {
            // アニメーションを実体化
            var characterGameObject = Instantiate(resourceObject, Vector3.zero, Quaternion.identity) as GameObject;
            if (characterGameObject != null) {
                scriptRoot = Script_SpriteStudio6_Root.Parts.RootGet(characterGameObject);
                if (scriptRoot != null) {
                    // 座標設定するためのGameObject作成
                    var characterRoot = new GameObject("Doll");
                    if (characterRoot == null) {
                        // 作成できないケース対応 
                        Destroy(characterGameObject);
                        scriptRoot = null;
                    }
                    else {
                        // 座標設定 
                        characterGameObject.transform.SetParent(characterRoot.transform);

                        // 自分の子に移動して座標を設定
                        characterRoot.transform.SetParent(transform);
                        characterRoot.transform.localPosition = positionOffset;
                        characterRoot.transform.localRotation = Quaternion.identity;
                        characterRoot.transform.localScale = characterScale;

                        //アニメーション再生
                        ChangeAnimation(AnimationPattern.Walk);
                    }
                }
            }
        }
    }

    // アニメーション 再生/変更 
    private void ChangeAnimation(AnimationPattern pattern) {
        if (scriptRoot == null) {
            return;
        }

        int timesPlay = 0;
        if (scriptRoot != null) {
            switch (pattern) {
                case AnimationPattern.Walk:
                    timesPlay = 0;    // ループ再生 
                    break;
                case AnimationPattern.Attack:
                    timesPlay = 1;    // 1回だけ再生 
                    break;
                default:
                    break;
            }
            scriptRoot.AnimationPlay(-1, (int)pattern, timesPlay);
        }
    }

    // アニメーションが再生中か停止中(エラー含)か取得します
    private bool IsAnimationPlay() {
        if (scriptRoot != null) {
            // 再生回数を取得して、プレイ終了かを判断します
            var remain = scriptRoot.PlayTimesGetRemain(0);
            if (remain >= 0) {
                return true;
            }
        }
        return false;
    }
    #endregion
}
