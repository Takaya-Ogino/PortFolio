using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 箱の挙動を管理します
public class Box : MonoBehaviour {
    //箱が掴まれているかを判定するフラグ
    public bool beingGrabed = false;
    // 浮いているどうかのフラグ
    public bool isFloating;
    //箱のX座標を格納します
    float xpos;

    // 接地しているか判定するコライダー
    [SerializeField]
    private BoxCollider2D groundChecker = null;

    // コンポーネント取得用の変数
    FixedJoint2D fixedjoint2D;

    // 地面判定用の対象レイヤーを指定します
    [SerializeField]
    private LayerMask groundLayer;

    // Start is called before the first frame update
    void Start() {
        // コンポーネント取得
        fixedjoint2D = GetComponent<FixedJoint2D>();

        // 開始時のx座標を格納します
        xpos = transform.position.x;
    }

    // Update is called once per frame
    void Update() {

        //掴まれていない時には
        if (!beingGrabed) {
            // jointをfalseにします
            fixedjoint2D.enabled = false;
            //位置を固定します
            transform.position = new Vector2(xpos, transform.position.y);
        }
        //掴まれている時には
        else if (beingGrabed) {
            // jointをtrueにします
            fixedjoint2D.enabled = true;
            //X座標を更新します
            xpos = transform.position.x;
        }
    }

    private void FixedUpdate() {
        // 地面との交差判定
        Vector2 point = groundChecker.transform.position;
        point += groundChecker.offset;
        var result = Physics2D.OverlapBox(
            point,
            groundChecker.size,
            groundChecker.transform.rotation.eulerAngles.z,
            groundLayer);

        // 接地している状態
        if (result) {
            isFloating = false;
        }
        // 接地していない状態
        else {
            isFloating = true;
        }

    }
}
